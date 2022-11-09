#include <Core/Maps/Map.hpp>

#include <BLIB/Media/Audio.hpp>
#include <BLIB/Render/Cameras/FollowCamera.hpp>
#include <Core/Cameras/Util.hpp>
#include <Core/Events/Maps.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Scripts/MapChangeContext.hpp>
#include <Core/Scripts/MapEventContext.hpp>
#include <Core/Systems/Systems.hpp>
#include <cmath>

namespace core
{
namespace map
{
namespace loaders
{
class LegacyMapLoader : public bl::serial::binary::SerializerVersion<Map> {
public:
    virtual bool read(Map& result, bl::serial::binary::InputStream& input) const override {
        if (!input.read(result.nameField)) return false;
        if (!input.read(result.tilesetField)) return false;
        if (!input.read(result.playlistField)) return false;
        if (!input.read(result.loadScriptField)) return false;
        if (!input.read(result.unloadScriptField)) return false;

        BL_LOG_INFO << "Legacy map: " << result.nameField;

        std::uint32_t width, height;
        std::uint16_t layerCount, firstTopLayer, firstYSortLayer;
        if (!input.read(width)) return false;
        if (!input.read(height)) return false;
        if (!input.read(layerCount)) return false;
        if (!input.read(firstYSortLayer)) return false;
        if (!input.read(firstTopLayer)) return false;
        result.levels.resize(1);
        result.levels.front().init(width,
                                   height,
                                   firstYSortLayer,
                                   firstTopLayer - firstYSortLayer,
                                   layerCount - firstTopLayer);
        result.transitionField.setSize(width, height, LevelTransition::None);

        std::uint8_t weatherType;
        if (!input.read(weatherType)) return false;
        result.weatherField = static_cast<Weather::Type>(weatherType);

        std::uint16_t ambientLight;
        if (!input.read(ambientLight)) return false;
        const bool sunlight = ambientLight == 255;
        result.lightingSystem().adjustForSunlight(sunlight);
        result.lightingSystem().setAmbientLevel(75, sunlight ? 255 : 255 - ambientLight);
        result.lightingSystem().legacyResize({static_cast<int>(width), static_cast<int>(height)});

        std::uint16_t catchZoneCount;
        if (!input.read(catchZoneCount)) return false;
        for (unsigned int i = 0; i < catchZoneCount; ++i) {
            std::int32_t left, top, width, height;
            if (!input.read(left)) return false;
            if (!input.read(top)) return false;
            if (!input.read(width)) return false;
            if (!input.read(height)) return false;

            std::uint16_t pplCount;
            std::string trash;
            if (!input.read(pplCount)) return false;
            for (unsigned int j = 0; j < pplCount; ++j) {
                if (!input.read(trash)) return false;
            }
        }

        for (unsigned int x = 0; x < width; ++x) {
            for (unsigned int y = 0; y < height; ++y) {
                std::uint8_t c = static_cast<std::uint8_t>(Collision::Open);
                if (!input.read(c)) return false;
                result.levels.front().collisionLayer().set(x, y, static_cast<Collision>(c));
            }
        }

        for (unsigned int x = 0; x < width; ++x) {
            for (unsigned int y = 0; y < height; ++y) {
                std::uint8_t c = 0;
                if (!input.read(c)) return false;
                result.levels.front().catchLayer().set(x, y, c);
            }
        }

        for (unsigned int i = 0; i < layerCount; ++i) {
            for (unsigned int x = 0; x < width; ++x) {
                for (unsigned int y = 0; y < height; ++y) {
                    std::uint16_t id;
                    std::uint8_t isAnim;
                    if (!input.read(isAnim)) return false;
                    if (!input.read(id)) return false;

                    const unsigned int actualX = isAnim ? x - 1 : x;
                    const unsigned int actualY = isAnim ? y - 1 : y;
                    Tile* tile                 = nullptr;

                    if (i < firstYSortLayer) {
                        tile = &result.levels.front().bottomLayers()[i].getRef(actualX, actualY);
                    }
                    else if (i < firstTopLayer) {
                        tile = &result.levels.front().ysortLayers()[i - firstYSortLayer].getRef(
                            actualX, actualY);
                    }
                    else {
                        tile = &result.levels.front().topLayers()[i - firstTopLayer].getRef(
                            actualX, actualY);
                    }

                    if (tile->id() == Tile::Blank) {
                        tile->setDataOnly(static_cast<Tile::IdType>(id), static_cast<bool>(isAnim));
                    }
                }
            }
        }

        std::uint16_t spawnCount;
        if (!input.read(spawnCount)) return false;
        for (unsigned int i = 0; i < spawnCount; ++i) {
            std::uint16_t id;
            std::uint32_t x, y; // in pixels and off by 32 lol
            std::uint8_t dir;
            if (!input.read(id)) return false;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            if (!input.read(dir)) return false;
            component::Position pos;
            pos.direction = static_cast<component::Direction>(dir);
            pos.setTiles(
                {static_cast<int>(std::floor(x / 32)), static_cast<int>(std::floor(y / 32))});
            result.spawns.try_emplace(id, id, pos);
        }

        std::uint16_t characterCount;
        if (!input.read(characterCount)) return false;
        for (unsigned int i = 0; i < characterCount; ++i) {
            std::string file;
            std::uint32_t x, y;
            std::uint8_t dir;
            if (!input.read(file)) return false;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            if (!input.read(dir)) return false;
            result.characterField.emplace_back(
                component::Position(
                    0, sf::Vector2i(x / 32, y / 32), static_cast<component::Direction>(dir)),
                file);
        }

        std::uint16_t itemCount;
        if (!input.read(itemCount)) return false;
        for (unsigned int i = 0; i < itemCount; ++i) {
            std::uint32_t x, y;
            Item item;
            item.level = 0;
            if (!input.read(item.id)) return false;
            if (!input.read(item.mapId)) return false;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            item.position = sf::Vector2i(x / 32 - 1, y / 32 - 1);
            if (item.id > 500) {
                item.id      = item.id - 500;
                item.visible = false;
            }
            else {
                item.visible = true;
            }
            result.itemsField.push_back(item);
        }

        std::uint16_t lightCount;
        if (!input.read(lightCount)) return false;
        for (unsigned int i = 0; i < lightCount; ++i) {
            Light light;
            std::uint32_t x, y;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            if (!input.read(light.radius)) return false;
            light.position = sf::Vector2i(x, y);
            result.lighting.addLight(light, true);
        }

        std::uint16_t eventCount;
        if (!input.read(eventCount)) return false;
        for (unsigned int i = 0; i < eventCount; ++i) {
            std::uint8_t trigger, unused;
            std::uint32_t x, y;
            std::uint16_t w, h;
            std::string script;
            if (!input.read(script)) return false;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            if (!input.read(w)) return false;
            if (!input.read(h)) return false;
            if (!input.read(unused)) return false;
            if (!input.read(trigger)) return false;

            // in pixels for some reason
            x /= 32;
            y /= 32;

            // separate embedded legacy scripts out into files to make conversion easier
            if (bl::util::FileUtil::getExtension(script) != "psc") {
                bl::script::Script test(script);
                if (!test.valid() && !script.empty()) {
                    const std::string filename =
                        "legacy_event_" + std::to_string(x) + "_" + std::to_string(y) + ".psc";
                    const std::string path =
                        bl::util::FileUtil::joinPath(Properties::ScriptPath(), result.name());
                    if (!bl::util::FileUtil::createDirectory(path))
                        BL_LOG_ERROR << "Failed to mkdir: " << path;
                    const std::string fullfile = bl::util::FileUtil::joinPath(path, filename);
                    std::ofstream output(fullfile.c_str());
                    output << script;
                    script = bl::util::FileUtil::joinPath(result.name(), filename);
                    BL_LOG_WARN << "Legacy script requires conversion: " << fullfile;
                }
            }

            if (!script.empty()) {
                result.eventsField.emplace_back(script,
                                                sf::Vector2i(x, y),
                                                sf::Vector2i(w, h),
                                                static_cast<Event::Trigger>(trigger));
            }
        }

        return true;
    }

    virtual bool write(const Map&, bl::serial::binary::OutputStream&) const override {
        // unimplemented
        return false;
    }
};

class PrimaryMapLoader : public bl::serial::binary::SerializerVersion<Map> {
    using Serializer = bl::serial::binary::Serializer<Map>;

public:
    virtual bool read(Map& result, bl::serial::binary::InputStream& input) const override {
        return Serializer::deserialize(input, result);
    }

    virtual bool write(const Map& map, bl::serial::binary::OutputStream& output) const override {
        return Serializer::serialize(output, map);
    }
};

} // namespace loaders
namespace
{
using VersionedSerializer = bl::serial::binary::VersionedSerializer<Map, loaders::LegacyMapLoader,
                                                                    loaders::PrimaryMapLoader>;
}

std::vector<Town> Map::flymapTowns;

Map::Map()
: weatherField(Weather::None)
, systems(nullptr)
, eventRegions(100.f, 100.f, 100.f, 100.f)
, activated(false) {
    cover.setFillColor(sf::Color::Black);
}

bool Map::enter(system::Systems& game, std::uint16_t spawnId, const std::string& prevMap,
                const component::Position& prevPlayerPos) {
    BL_LOG_INFO << "Entering map " << nameField << " at spawn " << spawnId;

    systems = &game;
    size    = {static_cast<int>(levels.front().bottomLayers().front().width()),
            static_cast<int>(levels.front().bottomLayers().front().height())};
    game.engine().eventBus().dispatch<event::MapSwitch>({*this});

    // Spawn player
    auto spawnIt                 = spawns.find(spawnId);
    component::Position spawnPos = prevPlayerPos;
    if (spawnId != 0 && spawnIt != spawns.end()) { spawnPos = spawnIt->second.position; }
    else if (spawnId == 0 && spawnIt != spawns.end()) {
        BL_LOG_WARN << "Spawn id 0 is reserved, falling back on default behavior";
    }
    else if (spawnId != 0) {
        BL_LOG_ERROR << "Invalid spawn id: " << spawnId;
        return false;
    }
    if (!game.player().spawnPlayer(spawnPos)) {
        BL_LOG_ERROR << "Failed to spawn player";
        return false;
    }
    setupCamera(game);
    currentTown = getTown(spawnPos.positionTiles());
    enterTown(currentTown);

    // Activate camera and weather
    weather.activate(game.engine().renderSystem().cameras().getCurrentViewport());

    // One time activation if not yet activated
    if (!activated) {
        activated = true;
        BL_LOG_INFO << "Activating map " << nameField;

        // Load tileset and init tiles
        tileset = Resources::tilesets().load(tilesetField).data;
        if (!tileset) return false;
        tileset->activate();
        for (LayerSet& level : levels) { level.activate(*tileset); }

        // Initialize weather, lighting, and wild peoplemon
        weather.set(weatherField);
        lighting.activate(size);

        // Load and parse scripts
        script::LegacyWarn::warn(loadScriptField);
        script::LegacyWarn::warn(unloadScriptField);
        onEnterScript.reset(new bl::script::Script(loadScriptField));
        onExitScript.reset(new bl::script::Script(unloadScriptField));

        // Build event zones data structure
        eventRegions.setSize(static_cast<float>(size.x * Properties::PixelsPerTile()),
                             static_cast<float>(size.y * Properties::PixelsPerTile()),
                             static_cast<float>(Properties::WindowWidth()),
                             static_cast<float>(Properties::WindowHeight()));
        for (const Event& event : eventsField) {
            eventRegions.add(static_cast<float>(event.position.x * Properties::PixelsPerTile()),
                             static_cast<float>(event.position.y * Properties::PixelsPerTile()),
                             &event);
        }

        BL_LOG_INFO << nameField << " activated";
    }

    // Ensure lighting is updated for time
    lighting.subscribe(game.engine().eventBus());

    // Spawn npcs and trainers
    for (const CharacterSpawn& spawn : characterField) {
        if (game.entity().spawnCharacter(spawn) == bl::entity::InvalidEntity) {
            BL_LOG_WARN << "Failed to spawn character: " << spawn.file;
        }
    }

    // Spawn items
    for (const Item& item : itemsField) { game.entity().spawnItem(item); }

    setupCamera(game); // TODO - position address isn't stable

    // Run on load script
    onEnterScript->resetContext(script::MapChangeContext(game, prevMap, nameField, spawnId));
    onEnterScript->run(&game.engine().scriptManager());

    // subscribe to get entity position updates
    game.engine().eventBus().subscribe(this);

    // start music
    playlistHandle = bl::audio::AudioSystem::getOrLoadPlaylist(
        bl::util::FileUtil::joinPath(Properties::PlaylistPath(), playlistField));
    if (playlistHandle != bl::audio::AudioSystem::InvalidHandle) {
        bl::audio::AudioSystem::replacePlaylist(playlistHandle);
    }

    game.engine().eventBus().dispatch<event::MapEntered>({*this});
    BL_LOG_INFO << "Entered map: " << nameField;

    return true;
}

void Map::exit(system::Systems& game, const std::string& newMap) {
    BL_LOG_INFO << "Exiting map " << nameField;
    game.engine().eventBus().dispatch<event::MapExited>({*this});

    // unsubscribe from entity events
    game.engine().eventBus().unsubscribe(this);

    // shut down light system
    lighting.unsubscribe();

    // run exit script
    if (onExitScript) {
        onExitScript->resetContext(script::MapChangeContext(game, nameField, newMap, 0));
        onExitScript->run(&game.engine().scriptManager());
    }

    // TODO - pause weather

    BL_LOG_INFO << "Exited map: " << nameField;
}

void Map::setupCamera(system::Systems& systems) {
    systems.engine().renderSystem().cameras().setViewportConstraint(
        {sf::Vector2f{0.f, 0.f}, sizePixels()});
    systems.engine().renderSystem().cameras().clearAndReplace(
        bl::render::camera::FollowCamera::create(
            camera::getPositionPointer(systems, systems.player().player()),
            Properties::WindowSize()));
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

void Map::update(system::Systems& systems, float dt) {
    tileset->update(dt);
    for (LayerSet& level : levels) { level.update(renderRange, dt); }
    weather.update(systems, dt);
    lighting.update(dt);
}

void Map::render(sf::RenderTarget& target, float residual,
                 const EntityRenderCallback& entityCb) const {
    const sf::View& view = target.getView();
    cover.setPosition(view.getCenter());
    cover.setSize(view.getSize());
    cover.setOrigin(view.getSize() * 0.5f);
    target.draw(cover, {sf::BlendNone});

    refreshRenderRange(target.getView());

    const auto renderRow = [&target, residual, this](const TileLayer& layer, int row) {
        for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
            layer.get(x, row).render(target, residual);
        }
    };

    const auto renderSorted = [&target, residual, this](const SortedLayer& layer, int row) {
        for (int x = renderRange.left; x < renderRange.left + renderRange.width; ++x) {
            Tile* t = layer(x, row);
            if (t) t->render(target, residual);
        }
    };

    for (unsigned int i = 0; i < levels.size(); ++i) {
        const LayerSet& level = levels[i];

        for (const TileLayer& layer : level.bottomLayers()) {
            for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
                renderRow(layer, y);
            }
        }
        for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
            for (const SortedLayer& layer : level.renderSortedLayers()) { renderSorted(layer, y); }
            entityCb(i, y, renderRange.left, renderRange.left + renderRange.width);
        }
        for (const TileLayer& layer : level.topLayers()) {
            for (int y = renderRange.top; y < renderRange.top + renderRange.height; ++y) {
                renderRow(layer, y);
            }
        }
    }

    weather.render(target, residual);
    const_cast<Map*>(this)->lighting.render(target);
}

bool Map::load(const std::string& file) {
    BL_LOG_INFO << "Loading map " << file;
    renderRange = sf::IntRect(0, 0, 1, 1);

    std::string path = bl::util::FileUtil::getExtension(file) == "map" ? file : file + ".map";
    if (!bl::util::FileUtil::exists(path))
        path = bl::util::FileUtil::joinPath(Properties::MapPath(), path);
    if (!bl::util::FileUtil::exists(path)) {
        BL_LOG_ERROR << "Failed to find map '" << file << "'. Tried '" << path << "'";
        return false;
    }
    bl::serial::binary::InputFile input(path);
    if (!VersionedSerializer::read(input, *this)) return false;

    defaultTown.name     = nameField;
    defaultTown.playlist = playlistField;
    defaultTown.weather  = weatherField;
    if (townTiles.getWidth() != levels.front().bottomLayers().front().width() ||
        townTiles.getHeight() != levels.front().bottomLayers().front().height()) {
        townTiles.setSize(levels.front().bottomLayers().front().width(),
                          levels.front().bottomLayers().front().height(),
                          0);
    }
    isWorldMap = name() == "Worldmap";
    return true;
}

bool Map::save(const std::string& file) {
    bl::serial::binary::OutputFile output(
        bl::util::FileUtil::joinPath(Properties::MapPath(), file));
    return VersionedSerializer::write(output, *this);
}

bool Map::contains(const component::Position& pos) const {
    return pos.positionTiles().x >= 0 && pos.positionTiles().y >= 0 &&
           pos.positionTiles().x < size.x && pos.positionTiles().y < size.y &&
           pos.level < levels.size();
}

component::Position Map::adjacentTile(const component::Position& pos,
                                      component::Direction dir) const {
    component::Position npos = pos.move(dir);
    if (npos.positionTiles() == pos.positionTiles()) return npos;

    // Handle up transitions (move out of tile)
    if (contains(pos)) {
        switch (transitionField(pos.positionTiles().x, pos.positionTiles().y)) {
        case LevelTransition::HorizontalRightDown:
            if (dir == component::Direction::Left) npos.level += 1;
            break;
        case LevelTransition::HorizontalRightUp:
            if (dir == component::Direction::Right) npos.level += 1;
            break;
        case LevelTransition::VerticalTopDown:
            if (dir == component::Direction::Down) npos.level += 1;
            break;
        case LevelTransition::VerticalTopUp:
            if (dir == component::Direction::Up) npos.level += 1;
            break;
        default:
            break;
        }
    }

    if (contains(npos)) {
        switch (transitionField(npos.positionTiles().x, npos.positionTiles().y)) {
        case LevelTransition::HorizontalRightDown:
            if (dir == component::Direction::Right) npos.level -= 1;
            break;
        case LevelTransition::HorizontalRightUp:
            if (dir == component::Direction::Left) npos.level -= 1;
            break;
        case LevelTransition::VerticalTopDown:
            if (dir == component::Direction::Up) npos.level -= 1;
            break;
        case LevelTransition::VerticalTopUp:
            if (dir == component::Direction::Down) npos.level -= 1;
            break;
        default:
            break;
        }
    }

    if (npos.level >= levels.size()) {
        BL_LOG_WARN << "Bad level transition at (" << npos.positionTiles().x << ", "
                    << npos.positionTiles().y << ") to out of range level " << npos.level
                    << ". Number of levels: " << levels.size();
        npos.level = levels.size() - 1;
    }

    return npos;
}

bool Map::movePossible(const component::Position& pos, component::Direction dir) const {
    component::Position npos = pos.move(dir);
    if (npos.positionTiles() == pos.positionTiles()) return true;
    if (!contains(npos)) return false;

    switch (levels.at(npos.level)
                .collisionLayer()
                .get(npos.positionTiles().x, npos.positionTiles().y)) {
    case Collision::Blocked:
        return false;
    case Collision::Open:
        return true;
    case Collision::TopOpen:
        return dir == component::Direction::Down;
    case Collision::RightOpen:
        return dir == component::Direction::Left;
    case Collision::BottomOpen:
        return dir == component::Direction::Up;
    case Collision::LeftOpen:
        return dir == component::Direction::Right;
    case Collision::TopRightOpen:
        return dir == component::Direction::Down || dir == component::Direction::Left;
    case Collision::BottomRightOpen:
        return dir == component::Direction::Up || dir == component::Direction::Left;
    case Collision::BottomLeftOpen:
        return dir == component::Direction::Up || dir == component::Direction::Right;
    case Collision::TopLeftOpen:
        return dir == component::Direction::Down || dir == component::Direction::Right;
    case Collision::TopBottomOpen:
        return dir == component::Direction::Up || dir == component::Direction::Down;
    case Collision::LeftRightOpen:
        return dir == component::Direction::Right || dir == component::Direction::Left;
    case Collision::TopClosed:
        return dir != component::Direction::Down;
    case Collision::RightClosed:
        return dir != component::Direction::Left;
    case Collision::BottomClosed:
    case Collision::LedgeHop:
        return dir != component::Direction::Up;
    case Collision::LeftClosed:
        return dir != component::Direction::Right;
    case Collision::SurfRequired:
        return systems->player().state().bag.hasItem(item::Id::JesusShoes);
    case Collision::WaterfallRequired:
        return systems->player().state().bag.hasItem(item::Id::JesusShoesUpgrade);
    default:
        BL_LOG_WARN << "Bad collision at (" << npos.positionTiles().x << ", "
                    << npos.positionTiles().y << ")";
        return false;
    }
}

bool Map::isLedgeHop(const component::Position& pos, component::Direction dir) const {
    if (dir != component::Direction::Down) return false;
    if (!contains(pos)) return false;
    return levels.at(pos.level).collisionLayer().get(pos.positionTiles().x,
                                                     pos.positionTiles().y) == Collision::LedgeHop;
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

    const auto range = eventRegions.getCellAndNeighbors(movedEvent.position.positionPixels().x,
                                                        movedEvent.position.positionPixels().y);
    for (const auto& it : range) {
        const Event& e = *it.get();
        const sf::IntRect area(e.position, e.areaSize);
        const bool wasIn = area.contains(movedEvent.previousPosition.positionTiles());
        const bool isIn  = area.contains(movedEvent.position.positionTiles());

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
    }

    Town* newTown = getTown(movedEvent.position.positionTiles());
    if (newTown != currentTown) {
        currentTown = newTown;
        enterTown(currentTown);
    }
}

void Map::triggerAnimation(const component::Position& pos) {
    if (contains(pos)) {
        auto& level = levels[pos.level];
        for (auto& layer : level.bottomLayers()) {
            layer.getRef(pos.positionTiles().x, pos.positionTiles().y).step();
        }
        for (auto& layer : level.ysortLayers()) {
            layer.getRef(pos.positionTiles().x, pos.positionTiles().y).step();
        }
        for (auto& layer : level.topLayers()) {
            layer.getRef(pos.positionTiles().x, pos.positionTiles().y).step();
        }
    }
}

bool Map::interact(bl::entity::Entity interactor, const component::Position& pos) {
    const auto trigger = [this, interactor, &pos](const Event& event) {
        script::LegacyWarn::warn(event.script);
        BL_LOG_INFO << interactor << " triggered event at (" << pos.positionTiles().x << ", "
                    << pos.positionTiles().y << ")";
        bl::script::Script s(event.script,
                             script::MapEventContext(*systems, interactor, event, pos));
        s.run(&systems->engine().scriptManager());
    };

    const auto range =
        eventRegions.getCellAndNeighbors(pos.positionPixels().x, pos.positionPixels().y);
    for (const auto& it : range) {
        const Event& e = *it.get();
        const sf::IntRect area(e.position, e.areaSize);
        if (area.contains(pos.positionTiles()) && e.trigger == Event::Trigger::OnInteract) {
            trigger(e);
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
    renderRange = sf::IntRect(0, 0, 1, 1);
}

void Map::refreshRenderRange(const sf::View& view) const {
    static const sf::Vector2i ExtraRender =
        sf::Vector2i(Properties::ExtraRenderTiles(), Properties::ExtraRenderTiles());

    const sf::Vector2f cornerPixels = view.getCenter() - view.getSize() * 0.5f;
    sf::Vector2i corner =
        static_cast<sf::Vector2i>(cornerPixels) / Properties::PixelsPerTile() - ExtraRender;
    if (corner.x < 0) corner.x = 0;
    if (corner.y < 0) corner.y = 0;

    sf::Vector2i wsize =
        static_cast<sf::Vector2i>(view.getSize()) / Properties::PixelsPerTile() + ExtraRender * 2;
    if (corner.x + wsize.x > size.x) wsize.x = size.x - corner.x;
    if (corner.y + wsize.y > size.y) wsize.y = size.y - corner.y;
    renderRange = {corner, wsize};
}

Town* Map::getTown(const sf::Vector2i& pos) {
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

const CatchRegion* Map::getCatchRegion(const component::Position& pos) const {
    const auto& tiles = pos.positionTiles();
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
    bl::resource::Resource<Map>::Ref world = Resources::maps().load("WorldMap.map").data;
    if (!world) {
        BL_LOG_CRITICAL << "Failed to load world map";
        return;
    }

    flymapTowns = world->towns;
}

bool Map::canFlyFromHere() const { return isWorldMap; }

const component::Position* Map::getSpawnPosition(unsigned int spid) const {
    const auto sit = spawns.find(spid);
    return sit != spawns.end() ? &sit->second.position : nullptr;
}

const std::string& Map::getLocationName(const component::Position& pos) const {
    return const_cast<Map*>(this)->getTown(pos.positionTiles())->name;
}

} // namespace map
} // namespace core
