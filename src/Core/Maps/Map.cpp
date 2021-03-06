#include <Core/Maps/Map.hpp>

#include <Core/Events/Maps.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Scripts/MapChangeContext.hpp>
#include <Core/Scripts/MapEventContext.hpp>
#include <Core/Systems/Cameras/Follow.hpp>
#include <Core/Systems/Systems.hpp>
#include <cmath>

namespace core
{
namespace map
{
namespace loaders
{
class LegacyMapLoader : public bl::file::binary::VersionedPayloadLoader<Map> {
public:
    virtual bool read(Map& result, bl::file::binary::File& input) const override {
        if (!input.read(result.nameField.getValue())) return false;
        if (!input.read(result.tilesetField.getValue())) return false;
        if (!input.read(result.playlistField.getValue())) return false;
        if (!input.read(result.loadScriptField.getValue())) return false;
        if (!input.read(result.unloadScriptField.getValue())) return false;

        BL_LOG_INFO << "Legacy map: " << result.nameField.getValue();

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
        result.transitionField.getValue().setSize(width, height, LevelTransition::None);

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

            CatchZone zone;
            zone.area = sf::IntRect(left, top, width, height);

            std::uint16_t pplCount;
            if (!input.read(pplCount)) return false;
            zone.peoplemon.getValue().reserve(pplCount);
            for (unsigned int j = 0; j < pplCount; ++j) {
                zone.peoplemon.getValue().push_back({});
                if (!input.read(zone.peoplemon.getValue().back())) return false;
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
                std::uint8_t c = static_cast<std::uint8_t>(Catch::NoEncounter);
                if (!input.read(c)) return false;
                result.levels.front().catchLayer().set(x, y, static_cast<Catch>(c));
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
            result.spawnField.getValue().try_emplace(id, id, pos);
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
            result.characterField.getValue().emplace_back(
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
            if (!input.read(item.id.getValue())) return false;
            if (!input.read(item.mapId.getValue())) return false;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            item.position.getValue() = sf::Vector2i(x / 32, y / 32); // TODO - add/sub 1?
            if (item.id.getValue() > 500) {
                item.id      = item.id.getValue() - 500;
                item.visible = false;
            }
            else {
                item.visible = true;
            }
            result.itemsField.getValue().push_back(item);
        }

        std::uint16_t lightCount;
        if (!input.read(lightCount)) return false;
        for (unsigned int i = 0; i < lightCount; ++i) {
            Light light;
            std::uint32_t x, y;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            if (!input.read(light.radius.getValue())) return false;
            light.position.getValue() = sf::Vector2i(x, y);
            result.lightsField.getValue().addLight(light, true);
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
            if (bl::file::Util::getExtension(script) != "psc") {
                bl::script::Script test(script);
                if (!test.valid() && !script.empty()) {
                    const std::string filename =
                        "legacy_event_" + std::to_string(x) + "_" + std::to_string(y) + ".psc";
                    const std::string path =
                        bl::file::Util::joinPath(Properties::ScriptPath(), result.name());
                    if (!bl::file::Util::createDirectory(path))
                        BL_LOG_ERROR << "Failed to mkdir: " << path;
                    const std::string fullfile = bl::file::Util::joinPath(path, filename);
                    std::ofstream output(fullfile.c_str());
                    output << script;
                    script = bl::file::Util::joinPath(result.name(), filename);
                    BL_LOG_WARN << "Legacy script requires conversion: " << fullfile;
                }
            }

            if (!script.empty()) {
                result.eventsField.getValue().emplace_back(script,
                                                           sf::Vector2i(x, y),
                                                           sf::Vector2i(w, h),
                                                           static_cast<Event::Trigger>(trigger));
            }
        }

        return true;
    }

    virtual bool write(const Map&, bl::file::binary::File&) const override {
        // unimplemented
        return false;
    }
};

class PrimaryMapLoader : public bl::file::binary::VersionedPayloadLoader<Map> {
public:
    virtual bool read(Map& result, bl::file::binary::File& input) const override {
        return result.deserialize(input);
    }

    virtual bool write(const Map& map, bl::file::binary::File& output) const override {
        return map.serialize(output);
    }
};

} // namespace loaders
namespace
{
using VersionedLoader =
    bl::file::binary::VersionedFile<Map, loaders::LegacyMapLoader, loaders::PrimaryMapLoader>;

}

Map::Map()
: nameField(*this)
, loadScriptField(*this)
, unloadScriptField(*this)
, playlistField(*this)
, weatherField(*this)
, levelsField(*this)
, tilesetField(*this)
, spawnField(*this)
, characterField(*this)
, itemsField(*this)
, eventsField(*this)
, lightsField(*this)
, catchZonesField(*this)
, transitionField(*this)
, systems(nullptr)
, levels(levelsField.getValue())
, spawns(spawnField.getValue())
, lighting(lightsField.getValue())
, eventRegions(100.f, 100.f, 100.f, 100.f)
, activated(false) {
    cover.setFillColor(sf::Color::Black);
}

bool Map::enter(system::Systems& game, std::uint16_t spawnId, const std::string& prevMap) {
    BL_LOG_INFO << "Entering map " << nameField.getValue() << " at spawn " << spawnId;

    systems = &game;
    size    = {static_cast<int>(levels.front().bottomLayers().front().width()),
            static_cast<int>(levels.front().bottomLayers().front().height())};
    game.engine().eventBus().dispatch<event::MapSwitch>({*this});

    // TODO - load and push playlist

    // Spawn player
    auto spawnIt = spawns.find(spawnId);
    if (spawnIt != spawns.end()) {
        if (!game.player().spawnPlayer(spawnIt->second.position)) {
            BL_LOG_ERROR << "Failed to spawn player";
            return false;
        }
        game.cameras().clearAndReplace(
            system::camera::Follow::create(game, game.player().player()));

        // Activate camera and weather
        game.cameras().update(0.f);
        weather.activate(game.cameras().getArea());
    }
    else {
        BL_LOG_ERROR << "Invalid spawn id: " << spawnId;
        return false;
    }

    // One time activation if not yet activated
    if (!activated) {
        activated = true;
        BL_LOG_INFO << "Activating map " << nameField.getValue();

        // Load tileset and init tiles
        tileset = Resources::tilesets().load(tilesetField).data;
        if (!tileset) return false;
        tileset->activate();
        for (LayerSet& level : levels) { level.activate(*tileset); }

        // Initialize weather, lighting, and wild peoplemon
        weather.set(weatherField.getValue());
        lighting.activate(size);
        for (CatchZone& zone : catchZonesField.getValue()) { zone.activate(); }

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
        for (const Event& event : eventsField.getValue()) {
            eventRegions.add(
                static_cast<float>(event.position.getValue().x * Properties::PixelsPerTile()),
                static_cast<float>(event.position.getValue().y * Properties::PixelsPerTile()),
                &event);
        }

        BL_LOG_INFO << nameField.getValue() << " activated";
    }

    // Ensure lighting is updated for time
    lighting.subscribe(game.engine().eventBus());

    // Spawn npcs and trainers
    for (const CharacterSpawn& spawn : characterField.getValue()) {
        if (!game.entity().spawnCharacter(spawn)) {
            BL_LOG_WARN << "Failed to spawn character: " << spawn.file.getValue();
        }
    }

    // Spawn items
    for (const Item& item : itemsField.getValue()) { game.entity().spawnItem(item); }

    // Run on load script
    onEnterScript->resetContext(script::MapChangeContext(game, prevMap, nameField, spawnId));
    onEnterScript->run(&game.engine().scriptManager());

    // subscribe to get entity position updates
    game.engine().eventBus().subscribe(this);

    game.engine().eventBus().dispatch<event::MapEntered>({*this});
    BL_LOG_INFO << "Entered map: " << nameField.getValue();

    return true;
}

void Map::exit(system::Systems& game, const std::string& newMap) {
    BL_LOG_INFO << "Exiting map " << nameField.getValue();
    game.engine().eventBus().dispatch<event::MapExited>({*this});

    // unsubscribe from entity events
    game.engine().eventBus().unsubscribe(this);

    // shut down light system
    lighting.unsubscribe();

    // run exit script
    onExitScript->resetContext(script::MapChangeContext(game, nameField, newMap, 0));
    onExitScript->run(&game.engine().scriptManager());

    // TODO - pop/pause playlist (maybe make param?)
    // TODO - pause weather

    BL_LOG_INFO << "Exited map: " << nameField.getValue();
}

const std::string& Map::name() const { return nameField.getValue(); }

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
}

void Map::render(sf::RenderTarget& target, float residual,
                 const EntityRenderCallback& entityCb) const {
    const sf::View& view = target.getView();
    cover.setPosition(view.getCenter());
    cover.setSize(view.getSize());
    cover.setOrigin(view.getSize() * 0.5f);
    target.draw(cover, {sf::BlendNone});

    static const sf::Vector2i ExtraRender =
        sf::Vector2i(Properties::ExtraRenderTiles(), Properties::ExtraRenderTiles());

    const sf::Vector2f cornerPixels =
        target.getView().getCenter() - target.getView().getSize() / 2.f;
    sf::Vector2i corner =
        static_cast<sf::Vector2i>(cornerPixels) / Properties::PixelsPerTile() - ExtraRender;
    if (corner.x < 0) corner.x = 0;
    if (corner.y < 0) corner.y = 0;

    sf::Vector2i wsize =
        static_cast<sf::Vector2i>(target.getView().getSize()) / Properties::PixelsPerTile() +
        ExtraRender * 2;
    if (corner.x + wsize.x > size.x) wsize.x = size.x - corner.x;
    if (corner.y + wsize.y > size.y) wsize.y = size.y - corner.y;
    renderRange = {corner, wsize};

    const auto renderRow = [&target, residual, &corner, &wsize](const TileLayer& layer, int row) {
        for (int x = corner.x; x < corner.x + wsize.x; ++x) {
            layer.get(x, row).render(target, residual);
        }
    };

    const auto renderSorted = [&target, residual, &corner, &wsize](const SortedLayer& layer,
                                                                   int row) {
        for (int x = corner.x; x < corner.x + wsize.x; ++x) {
            Tile* t = layer(x, row);
            if (t) t->render(target, residual);
        }
    };

    for (unsigned int i = 0; i < levels.size(); ++i) {
        LayerSet& level = levels[i];

        for (const TileLayer& layer : level.bottomLayers()) {
            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
        for (int y = corner.y; y < corner.y + wsize.y; ++y) {
            for (const SortedLayer& layer : level.renderSortedLayers()) { renderSorted(layer, y); }
            entityCb(i, y, corner.x, corner.x + wsize.x);
        }
        for (const TileLayer& layer : level.topLayers()) {
            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
    }

    weather.render(target, residual);
    const_cast<Map*>(this)->lighting.render(target);
}

bool Map::load(const std::string& file) {
    BL_LOG_INFO << "Loading map " << file;
    renderRange = sf::IntRect(0, 0, 1, 1);

    std::string path = bl::file::Util::getExtension(file) == "map" ? file : file + ".map";
    if (!bl::file::Util::exists(path)) path = bl::file::Util::joinPath(Properties::MapPath(), path);
    if (!bl::file::Util::exists(path)) {
        BL_LOG_ERROR << "Failed to find map '" << file << "'. Tried '" << path << "'";
        return false;
    }
    VersionedLoader loader;
    bl::file::binary::File input(path, bl::file::binary::File::Read);
    return loader.read(input, *this);
}

bool Map::save(const std::string& file) {
    bl::file::binary::File output(bl::file::Util::joinPath(Properties::MapPath(), file),
                                  bl::file::binary::File::Write);
    VersionedLoader loader;
    return loader.write(output, *this);
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
        switch (transitionField.getValue()(pos.positionTiles().x, pos.positionTiles().y)) {
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
        switch (transitionField.getValue()(npos.positionTiles().x, npos.positionTiles().y)) {
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
        return dir != component::Direction::Up;
    case Collision::LeftClosed:
        return dir != component::Direction::Right;
    case Collision::SurfRequired:
        return false; // TODO - enforce surf. play anim?
    case Collision::WaterfallRequired:
        return false; // TODO - enforce waterfall. play anim?
    default:
        BL_LOG_WARN << "Bad collision at (" << npos.positionTiles().x << ", "
                    << npos.positionTiles().y << ")";
        return false;
    }
}

void Map::observe(const event::EntityMoved& movedEvent) {
    triggerAnimation(movedEvent.position);

    const auto trigger = [this, &movedEvent](const Event& event) {
        script::LegacyWarn::warn(event.script);
        BL_LOG_INFO << movedEvent.entity << " triggered event at (" << event.position.getValue().x
                    << ", " << event.position.getValue().y << ")";
        bl::script::Script s(
            event.script,
            script::MapEventContext(*systems, movedEvent.entity, event, movedEvent.position));
        s.run(&systems->engine().scriptManager());
    };

    const auto range = eventRegions.getCellAndNeighbors(movedEvent.position.positionPixels().x,
                                                        movedEvent.position.positionPixels().y);
    for (const auto& it : range) {
        const Event& e = *it.get();
        const sf::IntRect area(e.position.getValue(), e.areaSize.getValue());
        const bool wasIn = area.contains(movedEvent.previousPosition.positionTiles());
        const bool isIn  = area.contains(movedEvent.position.positionTiles());

        switch (e.trigger.getValue()) {
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
}

void Map::triggerAnimation(const component::Position& pos) {
    if (pos.level < levels.size()) {
        LayerSet& level = levels[pos.level];
        if (pos.positionTiles().x >= 0 && pos.positionTiles().y >= 0 &&
            pos.positionTiles().x < sizeTiles().x && pos.positionTiles().y < sizeTiles().y) {
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
        const sf::IntRect area(e.position.getValue(), e.areaSize.getValue());
        if (area.contains(pos.positionTiles()) &&
            e.trigger.getValue() == Event::Trigger::OnInteract) {
            trigger(e);
            return true;
        }
    }

    return false;
}

void Map::clear() {
    levelsField.getValue().clear();
    spawnField.getValue().clear();
    characterField.getValue().clear();
    itemsField.getValue().clear();
    eventsField.getValue().clear();
    lightingSystem().clear();
    catchZonesField.getValue().clear();
    transitionField.getValue().clear();
    eventRegions.clear();
    weatherField = Weather::None;
    weather.set(Weather::None, true);
    renderRange = sf::IntRect(0, 0, 1, 1);
}

} // namespace map
} // namespace core
