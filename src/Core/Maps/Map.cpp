#include <Core/Maps/Map.hpp>

#include <BLIB/Audio.hpp>
#include <BLIB/Cameras.hpp>
#include <Core/Events/Maps.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Scripts/MapChangeContext.hpp>
#include <Core/Scripts/MapEventContext.hpp>
#include <Core/Systems/Systems.hpp>
#include <cmath>
#include <numeric>

namespace core
{
namespace map
{
std::vector<Town> Map::flymapTowns;

Map::Map()
: weatherField(Weather::None)
, systems(nullptr)
, eventRegions({}, 1.f, 1.f) // no allocations
, activated(false) {}

Map::~Map() {
    // Clear batch buffers first for speedier cleanup than if tiles are destroyed first
    renderLevels.clear();
}

bool Map::enter(system::Systems& game, std::uint16_t spawnId, const std::string& prevMap,
                const bl::tmap::Position& prevPlayerPos) {
    BL_LOG_INFO << "Entering map " << nameField << " at spawn " << spawnId;

    systems = &game;
    bl::event::Dispatcher::dispatch<event::MapSwitch>({*this});

    // One time activation if not yet activated
    if (!activated) {
        activated = true;
        BL_LOG_INFO << "Activating map " << nameField;

        // Load tileset and init tiles
        tileset = TilesetManager::load(Tileset::getFullPath(tilesetField));
        if (!tileset) return false;
        tileset->activate(game.engine());

        // Prepare rendering
        if (!scene) { prepareRender(); }

        // Initialize weather, lighting, and wild peoplemon
        weather.set(weatherField);
        lighting.activate(getSceneLighting());

        // Load and parse scripts
        script::LegacyWarn::warn(loadScriptField);
        script::LegacyWarn::warn(unloadScriptField);
        onEnterScript.reset(new bl::script::Script(loadScriptField));
        onExitScript.reset(new bl::script::Script(unloadScriptField));

        // Build event zones data structure
        eventRegions.setSize({0.f,
                              0.f,
                              static_cast<float>(size.x * Properties::PixelsPerTile()),
                              static_cast<float>(size.y * Properties::PixelsPerTile())},
                             static_cast<float>(Properties::WindowWidth()),
                             static_cast<float>(Properties::WindowHeight()));
        for (const Event& event : eventsField) {
            eventRegions.add(sf::Vector2f(event.position * Properties::PixelsPerTile()), &event);
        }

        BL_LOG_INFO << nameField << " activated";
    }

    // Spawn player
    auto spawnIt                = spawns.find(spawnId);
    bl::tmap::Position spawnPos = prevPlayerPos;
    if (spawnId != 0 && spawnIt != spawns.end()) { spawnPos = spawnIt->second.position; }
    else if (spawnId == 0 && spawnIt != spawns.end()) {
        BL_LOG_WARN << "Spawn id 0 is reserved, falling back on default behavior";
    }
    else if (spawnId != 0) {
        BL_LOG_ERROR << "Invalid spawn id: " << spawnId;
        return false;
    }
    if (!game.player().spawnPlayer(spawnPos, *this)) {
        BL_LOG_ERROR << "Failed to spawn player";
        return false;
    }
    setupCamera(game);
    currentTown = getTown(spawnPos.position);
    enterTown(currentTown);

    // Activate camera and weather
    // TODO - weather.activate(game.engine().renderSystem().cameras().getCurrentViewport());

    // Ensure lighting is updated for time
    lighting.subscribe();

    // Spawn npcs and trainers
    for (const CharacterSpawn& spawn : characterField) {
        if (game.entity().spawnCharacter(spawn, *this) == bl::ecs::InvalidEntity) {
            BL_LOG_WARN << "Failed to spawn character: " << spawn.file;
        }
    }

    // Spawn items
    for (const Item& item : itemsField) { game.entity().spawnItem(item, *this); }

    setupCamera(game);

    // Run on load script
    onEnterScript->resetContext(script::MapChangeContext(game, prevMap, nameField, spawnId));
    onEnterScript->run(&game.engine().scriptManager());

    // subscribe to get entity position updates
    bl::event::Dispatcher::subscribe(this);

    // start music
    playlistHandle = bl::audio::AudioSystem::getOrLoadPlaylist(
        bl::util::FileUtil::joinPath(Properties::PlaylistPath(), playlistField));
    if (playlistHandle != bl::audio::AudioSystem::InvalidHandle) {
        bl::audio::AudioSystem::replacePlaylist(playlistHandle);
    }

    bl::event::Dispatcher::dispatch<event::MapEntered>({*this});
    BL_LOG_INFO << "Entered map: " << nameField;

    return true;
}

void Map::exit(system::Systems& game, const std::string& newMap) {
    BL_LOG_INFO << "Exiting map " << nameField;
    bl::event::Dispatcher::dispatch<event::MapExited>({*this});

    // unsubscribe from entity events
    bl::event::Dispatcher::unsubscribe(this);

    // shut down light system
    lighting.unsubscribe();

    // run exit script
    if (onExitScript) {
        onExitScript->resetContext(script::MapChangeContext(game, nameField, newMap, 0));
        onExitScript->run(&game.engine().scriptManager());
    }

    // remove our scene
    game.engine().renderer().getObserver().popScene();

    // TODO - pause weather

    BL_LOG_INFO << "Exited map: " << nameField;
}

void Map::setupCamera(system::Systems& systems) {
    bl::cam::Camera2D* cam = systems.engine().renderer().getObserver().setCamera<bl::cam::Camera2D>(
        glm::vec2{sizePixels().x, sizePixels().y},
        glm::vec2{Properties::WindowSize().x, Properties::WindowSize().y});
    cam->setController<bl::cam::c2d::ConstrainedFollower>(
        systems.engine().ecs(),
        systems.player().player(),
        sf::FloatRect(0.f, 0.f, sizePixels().x, sizePixels().y));
    cam->setNearAndFarPlanes(-getMinDepth(), 0.f);
}

const std::string& Map::name() const { return nameField; }

const sf::Vector2i& Map::sizeTiles() const { return size; }

sf::Vector2f Map::sizePixels() const {
    return {static_cast<float>(Properties::PixelsPerTile() * size.x),
            static_cast<float>(Properties::PixelsPerTile() * size.y)};
}

std::uint8_t Map::levelCount() const { return levels.size(); }

Weather& Map::weatherSystem() { return weather; }

LightingSystem& Map::lightingSystem() { return lighting; }

void Map::update(float dt) {
    weather.update(dt);
    lighting.update(dt);
    // TODO - refresh render range
}

std::string Map::getMapFile(const std::string& file) {
    std::string path = bl::util::FileUtil::getExtension(file) == "map" ? file : file + ".map";
    if (!bl::resource::FileSystem::resourceExists(path)) {
        path = bl::util::FileUtil::joinPath(Properties::MapPath(), path);
    }
    if (!bl::resource::FileSystem::resourceExists(path)) {
        BL_LOG_ERROR << "Failed to find map '" << file << "'. Tried '" << path << "'";
        return "";
    }
    return path;
}

bool Map::loadDev(std::istream& input) {
    if (!bl::serial::json::Serializer<Map>::deserializeStream(input, *this)) return false;
    finishLoad();
    return true;
}

bool Map::loadProd(bl::serial::binary::InputStream& input) {
    if (!bl::serial::binary::Serializer<Map>::deserialize(input, *this)) return false;
    finishLoad();
    return true;
}

void Map::finishLoad() {
    // renderRange          = sf::IntRect(0, 0, 1, 1);
    defaultTown.name     = nameField;
    defaultTown.playlist = playlistField;
    defaultTown.weather  = weatherField;
    size                 = {static_cast<int>(levels.front().collisionLayer().width()),
                            static_cast<int>(levels.front().collisionLayer().height())};
    if (townTiles.getWidth() != static_cast<unsigned int>(size.x) ||
        townTiles.getHeight() != static_cast<unsigned int>(size.y)) {
        townTiles.setSize(size.x, size.y, 0);
    }
    isWorldMap = name() == "Worldmap";
}

bool Map::save(const std::string& file) {
    if (!isWorldMap) {
        std::ofstream output(bl::util::FileUtil::startsWithPath(file, Properties::MapPath()) ?
                                 file.c_str() :
                                 bl::util::FileUtil::joinPath(Properties::MapPath(), file).c_str());
        return bl::serial::json::Serializer<Map>::serializeStream(output, *this, 4, 0);
    }
    else {
        // always save worldmap in binary otherwise it is too big
        bl::serial::binary::OutputFile output(
            bl::util::FileUtil::startsWithPath(file, Properties::MapPath()) ?
                file.c_str() :
                bl::util::FileUtil::joinPath(Properties::MapPath(), file).c_str());
        return bl::serial::binary::Serializer<Map>::serialize(output, *this);
    }
}

bool Map::saveBundle(bl::serial::binary::OutputStream& output,
                     bl::resource::bundle::FileHandlerContext& ctx) const {
    if (!bl::serial::binary::Serializer<Map>::serialize(output, *this)) return false;

    const auto addScript = [&ctx](const std::string& src) {
        std::string s = src;
        if (bl::script::Script::getFullScriptPath(s)) { ctx.addDependencyFile(s); }
    };
    addScript(loadScriptField);
    addScript(unloadScriptField);
    for (const Event& e : eventsField) { addScript(e.script); }

    const auto addCharacter = [&ctx](const std::string& cf) {
        std::string path = bl::util::FileUtil::joinPath(Properties::NpcPath(), cf);
        if (bl::util::FileUtil::exists(path)) { ctx.addDependencyFile(path); }
        else {
            path = bl::util::FileUtil::joinPath(Properties::TrainerPath(), cf);
            if (bl::util::FileUtil::exists(path)) { ctx.addDependencyFile(path); }
        }
    };
    for (const CharacterSpawn& s : characterField) { addCharacter(s.file); }

    return true;
}

bool Map::contains(const bl::tmap::Position& pos) const {
    return pos.position.x >= 0 && pos.position.y >= 0 && pos.position.x < size.x &&
           pos.position.y < size.y && pos.level < levels.size();
}

bl::tmap::Position Map::adjacentTile(const bl::tmap::Position& pos, bl::tmap::Direction dir) const {
    bl::tmap::Position npos = pos.move(dir);
    if (npos.position == pos.position) return npos;

    // Handle up transitions (move out of tile)
    if (contains(pos)) {
        switch (transitionField(pos.position.x, pos.position.y)) {
        case LevelTransition::HorizontalRightDown:
            if (dir == bl::tmap::Direction::Left) npos.level += 1;
            break;
        case LevelTransition::HorizontalRightUp:
            if (dir == bl::tmap::Direction::Right) npos.level += 1;
            break;
        case LevelTransition::VerticalTopDown:
            if (dir == bl::tmap::Direction::Down) npos.level += 1;
            break;
        case LevelTransition::VerticalTopUp:
            if (dir == bl::tmap::Direction::Up) npos.level += 1;
            break;
        default:
            break;
        }
    }

    if (contains(npos)) {
        switch (transitionField(npos.position.x, npos.position.y)) {
        case LevelTransition::HorizontalRightDown:
            if (dir == bl::tmap::Direction::Right) npos.level -= 1;
            break;
        case LevelTransition::HorizontalRightUp:
            if (dir == bl::tmap::Direction::Left) npos.level -= 1;
            break;
        case LevelTransition::VerticalTopDown:
            if (dir == bl::tmap::Direction::Up) npos.level -= 1;
            break;
        case LevelTransition::VerticalTopUp:
            if (dir == bl::tmap::Direction::Down) npos.level -= 1;
            break;
        default:
            break;
        }
    }

    if (npos.level >= levels.size()) {
        BL_LOG_WARN << "Bad level transition at (" << npos.position.x << ", " << npos.position.y
                    << ") to out of range level " << npos.level
                    << ". Number of levels: " << levels.size();
        npos.level = levels.size() - 1;
    }

    return npos;
}

bool Map::movePossible(const bl::tmap::Position& pos, bl::tmap::Direction dir) const {
    bl::tmap::Position npos = pos.move(dir);
    if (npos.position == pos.position) return true;
    if (!contains(npos)) return false;

    switch (levels.at(npos.level).collisionLayer().get(npos.position.x, npos.position.y)) {
    case Collision::Blocked:
        return false;
    case Collision::Open:
        return true;
    case Collision::TopOpen:
        return dir == bl::tmap::Direction::Down;
    case Collision::RightOpen:
        return dir == bl::tmap::Direction::Left;
    case Collision::BottomOpen:
        return dir == bl::tmap::Direction::Up;
    case Collision::LeftOpen:
        return dir == bl::tmap::Direction::Right;
    case Collision::TopRightOpen:
        return dir == bl::tmap::Direction::Down || dir == bl::tmap::Direction::Left;
    case Collision::BottomRightOpen:
        return dir == bl::tmap::Direction::Up || dir == bl::tmap::Direction::Left;
    case Collision::BottomLeftOpen:
        return dir == bl::tmap::Direction::Up || dir == bl::tmap::Direction::Right;
    case Collision::TopLeftOpen:
        return dir == bl::tmap::Direction::Down || dir == bl::tmap::Direction::Right;
    case Collision::TopBottomOpen:
        return dir == bl::tmap::Direction::Up || dir == bl::tmap::Direction::Down;
    case Collision::LeftRightOpen:
        return dir == bl::tmap::Direction::Right || dir == bl::tmap::Direction::Left;
    case Collision::TopClosed:
        return dir != bl::tmap::Direction::Down;
    case Collision::RightClosed:
        return dir != bl::tmap::Direction::Left;
    case Collision::BottomClosed:
    case Collision::LedgeHop:
        return dir != bl::tmap::Direction::Up;
    case Collision::LeftClosed:
        return dir != bl::tmap::Direction::Right;
    case Collision::SurfRequired:
        return systems->player().state().bag.hasItem(item::Id::JesusShoes);
    case Collision::WaterfallRequired:
        return systems->player().state().bag.hasItem(item::Id::JesusShoesUpgrade);
    default:
        BL_LOG_WARN << "Bad collision at (" << npos.position.x << ", " << npos.position.y << ")";
        return false;
    }
}

bool Map::isLedgeHop(const bl::tmap::Position& pos, bl::tmap::Direction dir) const {
    if (dir != bl::tmap::Direction::Down) return false;
    if (!contains(pos)) return false;
    return levels.at(pos.level).collisionLayer().get(pos.position.x, pos.position.y) ==
           Collision::LedgeHop;
}

void Map::observe(const event::EntityMoved& movedEvent) {
    triggerAnimation(movedEvent.position);

    if (movedEvent.entity != systems->player().player() || systems->flight().flying()) { return; }

    const auto trigger = [this, &movedEvent](const Event& event) {
        script::LegacyWarn::warn(event.script);
        BL_LOG_INFO << movedEvent.entity << " triggered event at (" << event.position.x << ", "
                    << event.position.y << ")";
        bl::script::Script s(
            event.script,
            script::MapEventContext(*systems, movedEvent.entity, event, movedEvent.position));
        s.run(&systems->engine().scriptManager());
    };

    const auto visitor = [&movedEvent, &trigger](const Event* ep) {
        const Event& e = *ep;
        const sf::IntRect area(e.position, e.areaSize);
        const bool wasIn = area.contains(
            {movedEvent.previousPosition.position.x, movedEvent.previousPosition.position.y});
        const bool isIn =
            area.contains({movedEvent.position.position.x, movedEvent.position.position.y});

        switch (e.trigger) {
        case Event::Trigger::OnEnter:
            if (!wasIn && isIn) trigger(e);
            break;

        case Event::Trigger::OnExit:
            if (wasIn && !isIn) trigger(e);
            break;

        case Event::Trigger::onEnterOrExit:
            if (wasIn != isIn) trigger(e);
            break;

        case Event::Trigger::WhileIn:
            if (isIn) trigger(e);
            break;

        default:
            break;
        }
    };
    eventRegions.forAllInCellAndNeighbors(
        movedEvent.position.getWorldPosition(Properties::PixelsPerTile()), visitor);

    Town* newTown = getTown(movedEvent.position.position);
    if (newTown != currentTown) {
        currentTown = newTown;
        enterTown(currentTown);
    }
}

void Map::triggerAnimation(const bl::tmap::Position& pos) {
    if (contains(pos)) {
        auto& level = levels[pos.level];
        for (auto& layer : level.bottomLayers()) {
            layer.getRef(pos.position.x, pos.position.y).step();
        }
        for (auto& layer : level.ysortLayers()) {
            layer.getRef(pos.position.x, pos.position.y).step();
        }
        for (auto& layer : level.topLayers()) {
            layer.getRef(pos.position.x, pos.position.y).step();
        }
    }
}

bool Map::interact(bl::ecs::Entity interactor, const bl::tmap::Position& pos) {
    const auto trigger = [this, interactor, &pos](const Event& event) {
        script::LegacyWarn::warn(event.script);
        BL_LOG_INFO << interactor << " triggered event at (" << pos.position.x << ", "
                    << pos.position.y << ")";
        bl::script::Script s(event.script,
                             script::MapEventContext(*systems, interactor, event, pos));
        s.run(&systems->engine().scriptManager());
    };

    bool found         = false;
    const auto visitor = [&trigger, &pos, &found](const Event* ep) -> bool {
        const Event& e = *ep;
        const sf::IntRect area(e.position, e.areaSize);
        if (area.contains({pos.position.x, pos.position.y}) &&
            e.trigger == Event::Trigger::OnInteract) {
            trigger(e);
            found = true;
            return true; // ends iteration
        }
        return false;
    };
    eventRegions.forAllInCellAndNeighbors(pos.getWorldPosition(Properties::PixelsPerTile()),
                                          visitor);
    if (found) return true;

    if (interactor == systems->player().player()) {
        const Collision col =
            levels[pos.level].collisionLayer().get(pos.position.x, pos.position.y);
        switch (col) {
        case Collision::LedgeHop:
            systems->hud().displayMessage(
                "This ledge is pretty tall. I'll have to find a way around.");
            return true;
        case Collision::SurfRequired:
            if (systems->player().state().bag.hasItem(item::Id::JesusShoes)) {
                systems->hud().displayMessage(
                    "I bet the shoes I'm wearing will let me walk right over this!");
            }
            else { systems->hud().displayMessage("There's no way I can walk on water! Unless..."); }
            return true;
        case Collision::WaterfallRequired:
            if (systems->player().state().bag.hasItem(item::Id::JesusShoesUpgrade)) {
                systems->hud().displayMessage(
                    "With my super awesome upgrades Jesus Shoes I bet I can walk right up this!");
            }
            else {
                systems->hud().displayMessage(
                    "Even my Jesus Shoes can't get up here. Maybe there's a new model available.");
            }
            return true;
        default:
            break;
        }

        // check ledge hop down
        const bl::tmap::Position prev = pos.move(bl::tmap::oppositeDirection(pos.direction));
        const Collision oncol =
            levels[pos.level].collisionLayer().get(prev.position.x, prev.position.y);
        if (oncol == Collision::LedgeHop && pos.direction == bl::tmap::Direction::Down) {
            systems->hud().displayMessage("I think I can jump down without getting hurt, but I "
                                          "won't be able to get back up.");
            return true;
        }
    }

    return false;
}

void Map::clear() {
    levels.clear();
    spawns.clear();
    characterField.clear();
    itemsField.clear();
    eventsField.clear();
    lightingSystem().clear();
    catchRegionsField.clear();
    transitionField.clear();
    eventRegions.clear();
    weatherField = Weather::None;
    weather.set(Weather::None, true);
    cleanupRender();
    // renderRange = sf::IntRect(0, 0, 1, 1);
}

Town* Map::getTown(const glm::i32vec2& pos) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= size.x || pos.y >= size.y) { return &defaultTown; }
    const std::uint8_t i = townTiles(pos.x, pos.y);
    if (i == 0) return &defaultTown;
    if (static_cast<std::uint8_t>(i - 1) >= towns.size()) return &defaultTown;
    return &towns[i - 1];
}

void Map::enterTown(Town* town) {
    using bl::audio::AudioSystem;
    using bl::util::FileUtil;

    systems->hud().displayEntryCard(town->name);
    weatherSystem().set(town->weather);

    const AudioSystem::Handle plst = AudioSystem::getOrLoadPlaylist(
        FileUtil::joinPath(Properties::PlaylistPath(), town->playlist));
    if (plst != AudioSystem::InvalidHandle) { AudioSystem::replacePlaylist(plst, 1.5f, 1.5f); }

    if (spawns.find(town->pcSpawn) != spawns.end()) {
        systems->world().setWhiteoutMap(town->pcSpawn);
    }

    if (isWorldMap) { systems->player().state().visitedTowns.emplace(town->name); }
}

const CatchRegion* Map::getCatchRegion(const bl::tmap::Position& pos) const {
    const auto& tiles = pos.position;
    if (pos.level >= levels.size() || tiles.x < 0 ||
        static_cast<unsigned int>(tiles.x) >= levels.front().catchLayer().width() || tiles.y < 0 ||
        static_cast<unsigned int>(tiles.y) >= levels.front().catchLayer().height()) {
        BL_LOG_ERROR << "Out of bounds position: " << pos;
        return nullptr;
    }

    std::uint8_t ci = levels[pos.level].catchLayer().get(tiles.x, tiles.y);
    if (ci == 0) { return nullptr; }
    --ci; // convert to index
    if (ci >= catchRegionsField.size()) {
        BL_LOG_ERROR << "Bad catch zone index (" << static_cast<int>(ci)
                     << ") at position: " << pos;
        return nullptr;
    }
    return &catchRegionsField[ci];
}

const std::vector<Town>& Map::FlyMapTowns() {
    if (flymapTowns.empty()) loadFlymapTowns();
    return flymapTowns;
}

void Map::loadFlymapTowns() {
    bl::resource::Ref<Map> world = MapManager::load("WorldMap.map");
    if (!world) {
        BL_LOG_CRITICAL << "Failed to load world map";
        return;
    }

    flymapTowns = world->towns;
}

bool Map::canFlyFromHere() const { return isWorldMap; }

const bl::tmap::Position* Map::getSpawnPosition(unsigned int spid) const {
    const auto sit = spawns.find(spid);
    return sit != spawns.end() ? &sit->second.position : nullptr;
}

const std::string& Map::getLocationName(const bl::tmap::Position& pos) const {
    return const_cast<Map*>(this)->getTown(pos.position)->name;
}

void Map::prepareRender() {
    BL_LOG_INFO << "Generating map geometry...";
    scene = systems->engine().renderer().scenePool().allocateScene<bl::rc::scene::Scene2D>();

    tileset->activate(systems->engine());
    for (unsigned int i = 0; i < levels.size(); ++i) {
        auto& rl = renderLevels.emplace_back();
        rl.create(systems->engine(),
                  tileset->combinedTextures,
                  levels[i].layerCount(),
                  sf::Vector2u(size),
                  scene);

        for (unsigned int j = 0; j < levels[i].layerCount(); ++j) {
            for (unsigned int x = 0; x < size.x; ++x) {
                for (unsigned int y = 0; y < size.y; ++y) { setupTile(i, j, {x, y}); }
            }
        }
    }

    BL_LOG_INFO << "Map geometry generated";
}

void Map::setupTile(unsigned int level, unsigned int layer, const sf::Vector2u& pos) {
    Tile& tile = levels[level].getLayer(layer).getRef(pos.x, pos.y);
    if (tile.id() == Tile::Blank) { return; }
    const glm::vec2 offset(pos.x * Properties::PixelsPerTile(),
                           pos.y * Properties::PixelsPerTile());

    auto it = renderLevels.begin();
    std::advance(it, level);
    auto& zone         = it->getZone(levels[level], layer);
    const bool isYsort = &zone == &it->zones[RenderLevel::Ysort];

    const auto getBottomDepth = [this, &pos, layer, level, &tile]() {
        const unsigned int th = tileset->tileHeight(tile.id(), tile.isAnimation());
        unsigned int size     = th / Properties::PixelsPerTile();
        if (th % Properties::PixelsPerTile() != 0) { ++size; }
        return getDepthForPosition(level, pos.y + size, layer);
    };

    if (tile.isAnimation()) {
        const auto ait               = tileset->sharedAnimations.find(tile.id());
        bl::ecs::Entity playerEntity = bl::ecs::InvalidEntity;
        bl::gfx::DiscreteAnimation2DPlayer player;
        bool isBatchSlideshow = true;
        if (ait != tileset->sharedAnimations.end()) { playerEntity = ait->second.entity(); }
        else {
            auto anim = tileset->getAnim(tile.id());
            if (!anim) {
                BL_LOG_ERROR << "Failed to find animation for tile id: " << tile.id();
                tile.set(Tile::Blank, false);
                return;
            }
            if (anim->isSlideshow()) {
                player.create(
                    systems->engine(), anim, bl::gfx::DiscreteAnimation2DPlayer::Slideshow);
                // will be cleaned up when dependency is removed in ECS
                playerEntity = player.entity();
            }
            else {
                isBatchSlideshow = false;
                bl::gfx::Animation2D& vanim =
                    *tile.renderObject.emplace<std::shared_ptr<bl::gfx::Animation2D>>(
                        std::make_shared<bl::gfx::Animation2D>(systems->engine(), anim));
                vanim.addToScene(scene, bl::rc::UpdateSpeed::Static);
                vanim.getTransform().setPosition(offset);
                if (isYsort) {
                    const float topDepth    = getDepthForPosition(level, pos.y, layer);
                    const float bottomDepth = getBottomDepth();
                    vanim.getTransform().setDepth((topDepth + bottomDepth) * 0.5f);
                }
                else { vanim.getTransform().setDepth(getDepthForPosition(level, pos.y, layer)); }
            }
        }
        if (isBatchSlideshow) {
            bl::gfx::BatchSlideshowSimple& anim =
                tile.renderObject.emplace<bl::gfx::BatchSlideshowSimple>(
                    systems->engine(), zone.tileAnims, playerEntity);

            for (auto& v : anim.getVertices()) {
                v.pos.x += offset.x;
                v.pos.y += offset.y;
                if (!isYsort) { v.pos.z = getDepthForPosition(level, pos.y, layer); }
            }
            if (isYsort) {
                const float topDepth        = getDepthForPosition(level, pos.y, layer);
                const float bottomDepth     = getBottomDepth();
                anim.getVertices()[0].pos.z = topDepth;
                anim.getVertices()[1].pos.z = topDepth;
                anim.getVertices()[2].pos.z = bottomDepth;
                anim.getVertices()[3].pos.z = bottomDepth;
            }
            anim.commit();
        }
    }
    else {
        const sf::FloatRect src = tileset->getTileTextureBounds(tile.id());
        if (src.width < 0.f) {
            BL_LOG_ERROR << "Failed to find texture for tile id: " << tile.id();
            return;
        }
        bl::gfx::BatchSpriteSimple& sprite =
            tile.renderObject.emplace<bl::gfx::BatchSpriteSimple>();
        if (!isYsort) {
            sprite.create(zone.tileSprites, src);
            const float depth =
                isYsort ? getBottomDepth() : getDepthForPosition(level, pos.y, layer);
            for (auto& v : sprite.getVertices()) {
                v.pos.x += offset.x;
                v.pos.y += offset.y;
                v.pos.z = depth;
            }
        }
        else {
            // TODO - not correct
            const float halfHeight = src.height * 0.5f;
            const std::array<sf::FloatRect, 2> srcs(
                {sf::FloatRect(src.left, src.top, src.width, halfHeight),
                 sf::FloatRect(src.left, src.top + halfHeight, src.width, halfHeight)});
            sprite.create(zone.tileSprites, src);
            const float topDepth    = getDepthForPosition(level, pos.y, layer);
            const float bottomDepth = getBottomDepth();
            const float midDepth    = (topDepth + bottomDepth) * 0.5f;
            unsigned int i          = 0;
            for (auto& v : sprite.getVertices()) {
                if (i < 4) {
                    v.pos.x += offset.x;
                    v.pos.y += offset.y;
                    v.pos.z = bottomDepth;
                }
                else {
                    v.pos.x += offset.x;
                    v.pos.y += offset.y + halfHeight;
                    v.pos.z = bottomDepth;
                }
            }
        }
        sprite.commit();
    }
}

float Map::getDepthForPosition(unsigned int level, unsigned int y, int layer) const {
    if (level >= levels.size()) {
        BL_LOG_ERROR << "Got invalid level: " << level;
        return 0.f;
    }

    const unsigned int maxLayers = std::accumulate(
        levels.begin(),
        levels.end(),
        levels.front().layerCount(),
        [](unsigned int val, const LayerSet& level) { return std::max(val, level.layerCount()); });
    const float depthPerLevel = static_cast<float>(size.y * maxLayers);
    const float levelBias     = static_cast<float>(level) * depthPerLevel;

    const LayerSet& lvl = levels[level];
    if (layer < 0) {
        // TODO - not correct
        layer = lvl.bottomLayers().size() + lvl.ysortLayers().size() - 1;
        // y -= 1;
    }
    const float layerBias = static_cast<float>(layer * maxLayers);
    // idea: small bias for bottom, many bias (say size.y) for ysort, extreme bias for top

    return -(levelBias + layerBias + static_cast<float>(y));
}

float Map::getMinDepth() const {
    return getDepthForPosition(levels.size() - 1, size.y - 1, levels.back().layerCount() - 1);
}

void Map::cleanupRender() {
    renderLevels.clear();
    for (auto& level : levels) {
        for (unsigned int j = 0; j < level.layerCount(); ++j) {
            for (unsigned int x = 0; x < size.x; ++x) {
                for (unsigned int y = 0; y < size.y; ++y) {
                    level.getLayer(j).getRef(x, y).renderObject.emplace<std::monostate>();
                }
            }
        }
    }
}

void Map::setupEntityPosition(bl::ecs::Entity ent) {
    auto set = systems->engine()
                   .ecs()
                   .getComponentSet<bl::ecs::Require<bl::tmap::Position, bl::com::Transform2D>,
                                    bl::ecs::Optional<component::Renderable>>(ent);
    if (!set.isValid()) {
        BL_LOG_ERROR << "Cannot setup position for entity " << ent << ", missing components";
        return;
    }

    bl::com::Transform2D* transform = set.get<bl::com::Transform2D>();
    bl::tmap::Position* pos         = set.get<bl::tmap::Position>();
    pos->transform                  = transform;
    transform->setDepth(getDepthForPosition(pos->level, pos->position.y));
    pos->syncTransform(Properties::PixelsPerTile());

    component::Renderable* rc = set.get<component::Renderable>();
    if (rc) { rc->notifyMoveState(pos->direction, false, false); }
}

} // namespace map
} // namespace core
