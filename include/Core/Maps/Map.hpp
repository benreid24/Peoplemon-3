#ifndef CORE_MAPS_MAP_HPP
#define CORE_MAPS_MAP_HPP

#include <Core/Maps/CatchZone.hpp>
#include <Core/Maps/CharacterSpawn.hpp>
#include <Core/Maps/Event.hpp>
#include <Core/Maps/Item.hpp>
#include <Core/Maps/LayerSet.hpp>
#include <Core/Maps/LevelTransition.hpp>
#include <Core/Maps/LightingSystem.hpp>
#include <Core/Maps/Spawn.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Core/Maps/Weather.hpp>

#include <BLIB/Entities.hpp>
#include <BLIB/Files/Binary.hpp>
#include <BLIB/Resources.hpp>
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
namespace system
{
class Systems;
}

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
     * @param systems The primary game systems
     * @param spawnId The spawn to place the player at
     * @return True on success, false on error
     */
    bool enter(system::Systems& systems, std::uint16_t spawnId);

    /**
     * @brief Removes spawned entities and runs the on-unload script
     *
     * @param systems The primary game systems
     */
    void exit(system::Systems& systems);

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
     * @brief Returns the size of the map in tiles
     *
     */
    const sf::Vector2i& sizeTiles() const;

    /**
     * @brief Returns the size of the map in pixels
     *
     */
    sf::Vector2f sizePixels() const;

    /**
     * @brief Updates internal logic over the elapsed time
     *
     * * @param systems The primary game systems
     * @param dt Elapsed time in seconds since last update
     */
    void update(system::Systems& systems, float dt);

    /**
     * @brief Renders the map to the given target using its built-in View
     *
     * @param target The target to render to
     * @param residual Residual time between calls to update, in seconds
     */
    void render(sf::RenderTarget& target, float residual);

    /**
     * @brief Returns whether or not the map contains the given position
     *
     * @param position The position to check for
     * @return True if the position is within the map, false if not
     */
    bool contains(const component::Position& position) const;

    /**
     * @brief Returns the adjacent position to the given position when moving in the given
     *        direction. Does not take into account collisions
     *
     * @param pos The position that is being moved from
     * @param dir The direction that the movement is going
     * @return component::Position The adjacent tile with level transition if any
     */
    component::Position adjacentTile(const component::Position& pos,
                                     component::Direction dir) const;

    /**
     * @brief Returns whether or not a particular movement is possible. Does not take into account
     *        entities blocking the way
     *
     * @param position The position being moved from
     * @param dir The direction to move in
     * @return True if the move is allowed, false if it is not
     */
    bool movePossible(const component::Position& position, component::Direction dir) const;

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
    bl::file::binary::SerializableField<14, bl::container::Vector2D<LevelTransition>>
        transitionField;

    sf::Vector2i size;
    bl::resource::Resource<Tileset>::Ref tileset;
    std::vector<LayerSet>& levels;
    std::unordered_map<std::uint16_t, Spawn>& spawns;
    Weather weather;
    LightingSystem& lighting;

    bool activated;                                  // for weather continuity
    std::vector<bl::entity::Entity> spawnedEntities; // for cleanup
    sf::IntRect renderRange;

    friend class loaders::LegacyMapLoader;
};

} // namespace map
} // namespace core

#endif
