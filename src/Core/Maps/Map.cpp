#include <Core/Maps/Map.hpp>

namespace core
{
namespace map
{
namespace loaders
{
class LegacyMapLoader : public bl::file::binary::VersionedPayloadLoader<Map> {
public:
    virtual bool read(Map& result, bl::file::binary::File& input) const override {
        // TODO
        return false;
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
, lightingSystem(lightsField.getValue()) {}

} // namespace map
} // namespace core
