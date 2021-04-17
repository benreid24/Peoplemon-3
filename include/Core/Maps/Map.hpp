#ifndef CORE_MAPS_MAP_HPP
#define CORE_MAPS_MAP_HPP

#include <Core/Game/Systems.hpp>
#include <Core/Maps/CatchZone.hpp>
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
    /**
     * @brief Creates an empty Map
     *
     */
    Map();

    /**
     * @brief Loads the map from the given file. Will try to determine if the extension or path need
     *        to be added in order to support how map names used to be entered
     *
     * @param file The file to load from
     * @return True if the map was able to be loaded, false on error
     */
    bool load(const std::string& file);

    /**
     * @brief Saves the map to the given file. The path should be relative to the maps directory and
     *        include the .map extension
     *
     * @param file The file to save to
     * @return True if saved successfully, false on error
     */
    bool save(const std::string& file);

    /**
     * @brief Initializes runtime data structures and spawns entities into the game. Also runs the
     *        on-load script
     *
     * @param game The main game object
     * @param spawnId The spawn to place the player at
     * @return True on success, false on error
     */
    bool enter(game::Systems& game, std::uint16_t spawnId);

    /**
     * @brief Removes spawned entities and runs the on-unload script
     *
     * @param game The main game object
     */
    void exit(game::Systems& game);

    /**
     * @brief Returns a reference to the weather system in this map
     *
     */
    Weather& weatherSystem();

    /**
     * @brief Returns a reference to the lighting system in this map
     *
     */
    LightingSystem& lightingSystem();

    /**
     * @brief Updates internal logic over the elapsed time
     *
     * @param dt Elapsed time in seconds since last update
     */
    void update(float dt);

    /**
     * @brief Renders the map to the given target using its built-in View
     *
     * @param target The target to render to
     * @param residual Residual time between calls to update, in seconds
     */
    void render(sf::RenderTarget& target, float residual);

    // TODO - entity level transitions

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
    bl::file::binary::SerializableField<13, std::vector<CatchZone>> catchZonesField;

    sf::Vector2i size;
    Tileset tileset;
    std::vector<LayerSet>& levels;
    std::unordered_map<std::uint16_t, Spawn>& spawns;
    Weather weather;
    LightingSystem& lighting;

    bool activated;                                  // for weather continuity
    std::vector<bl::entity::Entity> spawnedEntities; // for cleanup
    float renderTime;

    friend class loaders::LegacyMapLoader;
};

} // namespace map
} // namespace core

#endif
