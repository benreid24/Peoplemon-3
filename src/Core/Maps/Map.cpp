#include <Core/Maps/Map.hpp>

#include <Core/Events/Maps.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
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
        result.lightingSystem().adjustForSunlight(ambientLight == 255);
        result.lightingSystem().setAmbientLevel(ambientLight);
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
                    if (i < firstYSortLayer) {
                        result.levels.front().bottomLayers()[i].getRef(x, y).setDataOnly(
                            id, static_cast<bool>(isAnim));
                    }
                    else if (i < firstTopLayer) {
                        result.levels.front()
                            .ysortLayers()[i - firstYSortLayer]
                            .getRef(x, y)
                            .setDataOnly(id, static_cast<bool>(isAnim));
                    }
                    else {
                        result.levels.front()
                            .topLayers()[i - firstTopLayer]
                            .getRef(x, y)
                            .setDataOnly(id, static_cast<bool>(isAnim));
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
            pos.setTiles({static_cast<int>(std::floor(x / 32)),
                          static_cast<int>(std::floor(y / 32))}); // TODO - add/subtract 1?
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
                sf::Vector2i(x, y), static_cast<component::Direction>(dir), file);
        }

        std::uint16_t itemCount;
        if (!input.read(itemCount)) return false;
        for (unsigned int i = 0; i < itemCount; ++i) {
            std::uint32_t x, y;
            Item item;
            if (!input.read(item.id.getValue())) return false;
            if (!input.read(item.mapId.getValue())) return false;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            item.position.getValue() = sf::Vector2i(x, y);
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
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            if (!input.read(w)) return false;
            if (!input.read(h)) return false;
            if (!input.read(unused)) return false;
            if (!input.read(trigger)) return false;
            result.eventsField.getValue().emplace_back(script,
                                                       sf::Vector2i(x, y),
                                                       sf::Vector2i(w, h),
                                                       static_cast<Event::Trigger>(trigger));
        }

        return true;
    }

    virtual bool write(const Map& map, bl::file::binary::File& output) const override {
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
, levels(levelsField.getValue())
, spawns(spawnField.getValue())
, lighting(lightsField.getValue())
, catchZonesField(*this)
, transitionField(*this)
, activated(false) {}

bool Map::enter(system::Systems& systems, std::uint16_t spawnId) {
    BL_LOG_INFO << "Entering map " << nameField.getValue() << " at spawn " << spawnId;
    // TODO - spawn entities
    // TODO - move player to spawn
    // TODO - load and push playlist
    // TODO - run onload script

    if (!activated) {
        activated = true;
        BL_LOG_INFO << "Activating map " << nameField.getValue();

        size = {static_cast<int>(levels.front().bottomLayers().front().width()),
                static_cast<int>(levels.front().bottomLayers().front().height())};

        tileset = Resources::tilesets().load(tilesetField).data;
        if (!tileset) return false;
        for (LayerSet& set : levels) { set.activate(*tileset); }
        tileset->activate();

        weather.activate({0, 0, 800, 600}); // TODO - use camera/spawn
        weather.set(weatherField.getValue());
        lighting.activate(size);
        for (CatchZone& zone : catchZonesField.getValue()) { zone.activate(); }

        BL_LOG_INFO << nameField.getValue() << " activated";
    }

    lighting.subscribe(systems.engine().eventBus());

    systems.engine().eventBus().dispatch<event::MapEntered>({*this});
    return true;
}

void Map::exit(system::Systems& game) {
    BL_LOG_INFO << "Exiting map " << nameField.getValue();
    game.engine().eventBus().dispatch<event::MapExited>({*this});
    lighting.unsubscribe();
    // TODO - despawn entities/items. handle picked up items
    // TODO - pop/pause playlist (maybe make param?)
    // TODO - pause weather
    // TODO - run on unload script
}

const sf::Vector2i& Map::sizeTiles() const { return size; }

sf::Vector2f Map::sizePixels() const {
    return {static_cast<float>(Properties::PixelsPerTile() * size.x),
            static_cast<float>(Properties::PixelsPerTile() * size.y)};
}

Weather& Map::weatherSystem() { return weather; }

LightingSystem& Map::lightingSystem() { return lighting; }

void Map::update(system::Systems& systems, float dt) {
    tileset->update(dt);
    for (LayerSet& level : levels) { level.update(renderRange, dt); }
    weather.update(systems, dt);
}

// TODO - special editor rendering for hiding levels and layers
void Map::render(sf::RenderTarget& target, float residual) {
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
    if (corner.x + wsize.x >= size.x) wsize.x = size.x - corner.x - 1;
    if (corner.y + wsize.y >= size.y) wsize.y = size.y - corner.y - 1;
    renderRange = {corner, wsize};

    const auto renderRow = [&target, residual, &corner, &wsize](TileLayer& layer, int row) {
        for (int x = corner.x; x < corner.x + wsize.x; ++x) {
            layer.get(x, row).render(target, residual);
        }
    };

    for (unsigned int i = 0; i < levels.size(); ++i) {
        LayerSet& level = levels[i];

        for (TileLayer& layer : level.bottomLayers()) {
            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
        for (TileLayer& layer : level.ysortLayers()) {
            for (int y = corner.y; y < corner.y + wsize.y; ++y) {
                renderRow(layer, y);
                // TODO - render row of entities here by level and row
            }
        }
        for (TileLayer& layer : level.topLayers()) {
            for (int y = corner.y; y < corner.y + wsize.y; ++y) { renderRow(layer, y); }
        }
    }

    weather.render(target, residual);
    lightingSystem().render(target);
}

bool Map::load(const std::string& file) {
    BL_LOG_INFO << "Loading map " << file;

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

} // namespace map
} // namespace core
