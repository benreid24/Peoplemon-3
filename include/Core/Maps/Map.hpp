#ifndef CORE_MAPS_MAP_HPP
#define CORE_MAPS_MAP_HPP

#include <Core/Maps/CharacterSpawn.hpp>
#include <Core/Maps/Event.hpp>
#include <Core/Maps/Item.hpp>
#include <Core/Maps/LayerSet.hpp>
#include <Core/Maps/LightingSystem.hpp>
#include <Core/Maps/Spawn.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Core/Maps/Weather.hpp>

#include <BLIB/Files/Binary.hpp>
#include <unordered_map>
#include <vector>

/**
 * @addtogroup Maps
 * @ingroup Core
 * @brief Collection of classes responsible for loading, editing, rendering, and updating maps
 *
 */

namespace core
{
/// Collection of classes responsible for loading, editing, rendering, and updating maps
namespace map
{
namespace loaders
{
/**
 * @brief Loads the original peoplemon format maps
 *
 * @ingroup Maps
 *
 */
class LegacyMapLoader;

/**
 * @brief Loads the new format maps
 *
 * @ingroup Maps
 *
 */
class PrimaryMapLoader;

} // namespace loaders

/**
 * @brief The primary map class that represents a usable map in the game
 *
 * @ingroup Maps
 *
 */
class Map : public bl::file::binary::SerializableObject {
public:
    Map();

    void resize(unsigned int width, unsigned int height, bool modTop, bool modRight);

private:
    bl::file::binary::SerializableField<1, std::string> nameField;
    bl::file::binary::SerializableField<2, std::string> loadScriptField;
    bl::file::binary::SerializableField<3, std::string> unloadScriptField;
    bl::file::binary::SerializableField<4, std::string> playlistField;
    bl::file::binary::SerializableField<5, Weather::Type> weatherField;
    bl::file::binary::SerializableField<6, std::vector<LayerSet>> levelsField;
    bl::file::binary::SerializableField<7, std::string> tilesetField;
    bl::file::binary::SerializableField<8, std::unordered_map<std::uint16_t, Spawn>> spawnField;
    bl::file::binary::SerializableField<9, std::vector<CharacterSpawn>> characterField;
    bl::file::binary::SerializableField<10, std::vector<Item>> itemsField;
    bl::file::binary::SerializableField<11, std::vector<Event>> eventsField;
    bl::file::binary::SerializableField<12, LightingSystem> lightsField;

    Tileset tileset;
    std::vector<LayerSet>& levels;
    std::unordered_map<std::uint16_t, Spawn>& spawns;
    Weather weather; // TODO - persist?
    LightingSystem& lightingSystem;

    friend class loaders::LegacyMapLoader;
};

} // namespace map
} // namespace core

#endif
