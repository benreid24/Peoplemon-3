#include <Core/Maps/Map.hpp>

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

        std::uint8_t weatherType;
        if (!input.read(weatherType)) return false;
        result.weatherField = static_cast<Weather::Type>(weatherType);

        std::uint16_t ambientLight;
        if (!input.read(ambientLight)) return false;
        result.lightingSystem().adjustForSunlight(ambientLight == 255);
        result.lightingSystem().setAmbientLevel(ambientLight);

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
                        result.levels.front().bottomLayers()[i].set(
                            x, y, {id, static_cast<bool>(isAnim)});
                    }
                    else if (i < firstTopLayer) {
                        result.levels.front().ysortLayers()[i - firstYSortLayer].set(
                            x, y, {id, static_cast<bool>(isAnim)});
                    }
                    else {
                        result.levels.front().topLayers()[i - firstTopLayer].set(
                            x, y, {id, static_cast<bool>(isAnim)});
                    }
                }
            }
        }

        std::uint16_t spawnCount;
        if (!input.read(spawnCount)) return false;
        for (unsigned int i = 0; i < spawnCount; ++i) {
            std::uint16_t id;
            std::uint32_t x, y; // pixels and off by 32 lol
            std::uint8_t dir;
            if (!input.read(id)) return false;
            if (!input.read(x)) return false;
            if (!input.read(y)) return false;
            if (!input.read(dir)) return false;
            entity::Position pos;
            pos.direction  = static_cast<entity::Direction>(dir);
            pos.position.x = std::floor(x / 32);
            pos.position.y = std::floor(y / 32); // TODO - add/subtract 1?
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
                sf::Vector2i(x, y), static_cast<entity::Direction>(dir), file);
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
, catchZonesField(*this) {}

} // namespace map
} // namespace core
