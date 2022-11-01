#ifndef CORE_MAPS_MAP_HPP
#define CORE_MAPS_MAP_HPP

#include <Core/Events/EntityMoved.hpp>
#include <Core/Maps/CatchRegion.hpp>
#include <Core/Maps/CharacterSpawn.hpp>
#include <Core/Maps/Event.hpp>
#include <Core/Maps/Item.hpp>
#include <Core/Maps/LayerSet.hpp>
#include <Core/Maps/LevelTransition.hpp>
#include <Core/Maps/LightingSystem.hpp>
#include <Core/Maps/Spawn.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Core/Maps/Town.hpp>
#include <Core/Maps/Weather.hpp>

#include <BLIB/Entities.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Media/Audio.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Scripts.hpp>
#include <BLIB/Serialization.hpp>
#include <functional>
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
class Map : public bl::event::Listener<event::EntityMoved> {
public:
    /**
     * @brief Function signature for the callback to render rows of entities
     *
     * @param level Which level to render entities from
     * @param row Which row to render entities from
     * @param minX Starting x coordinate to render entities at
     * @param maxX Ending x coordinate to render entities at, exclusive
     *
     */
    using EntityRenderCallback = std::function<void(std::uint8_t level, unsigned int row,
                                                    unsigned int minX, unsigned int maxX)>;

    /**
     * @brief Creates an empty Map
     *
     */
    Map();

    /**
     * @brief Destroy the Map
     *
     */
    virtual ~Map() = default;

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
     * @param prevMap The name of the map coming from
     * @param prevPlayerPos Used to spawn the player when coming back from a map or loading save
     * @return True on success, false on error
     */
    bool enter(system::Systems& systems, std::uint16_t spawnId, const std::string& prevMap,
               const component::Position& prevPlayerPos);

    /**
     * @brief Removes spawned entities and runs the on-unload script
     *
     * @param systems The primary game systems
     * @param newMap The name of the map going to
     */
    void exit(system::Systems& systems, const std::string& newMap);

    /**
     * @brief Configures the game camera for the player in the map
     * 
     * @param systems The main game systems
     */
    void setupCamera(system::Systems& systems);

    /**
     * @brief Returns the name of the map
     *
     */
    const std::string& name() const;

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
     * @brief Returns the number of levels in the map
     *
     */
    std::uint8_t levelCount() const;

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
     * @param entityCb Function to call to render entities at the correct times
     */
    virtual void render(sf::RenderTarget& target, float residual,
                        const EntityRenderCallback& entityCb) const;

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

    /**
     * @brief Test whether the given movement will be a ledge hop or not
     *
     * @param position The current position being moved away from
     * @param dir The direction being moved in
     * @return True if the move is a ledge hop, false otherwise
     */
    bool isLedgeHop(const component::Position& position, component::Direction dir) const;

    /**
     * @brief Event listener for moving entities. Used to trigger map events
     *
     * @param moveEvent The move event
     */
    virtual void observe(const event::EntityMoved& moveEvent) override;

    /**
     * @brief Lets entities interact with the map itself. This is called by the Interaction system
     *
     * @param interactor The entity doing the interact
     * @param interactPos The position being interacted with
     */
    bool interact(bl::entity::Entity interactor, const component::Position& interactPos);

    /**
     * @brief Returns the catch region at the given position if the position is on a catch tile
     *
     * @param position The position to search
     * @return CatchRegion* The catch region if on a catch tile and a region is there, or nullptr
     */
    const CatchRegion* getCatchRegion(const component::Position& position) const;

    /**
     * @brief Returns the set of towns that can be flown to
     *
     */
    static const std::vector<Town>& FlyMapTowns();

    /**
     * @brief Returns whether or not the player can fly from this map
     *
     */
    bool canFlyFromHere() const;

    /**
     * @brief Returns the position of the given player spawn, or nullptr if not found
     *
     * @param spawnId Id of the spawn to get the position for
     * @return const component::Position* Pointer to the position to spawn at. May be nullptr
     */
    const component::Position* getSpawnPosition(unsigned int spawnId) const;

    /**
     * @brief Returns the name of the town or route at the given position
     *
     * @param pos The position to check for
     * @return const std::string& The name of the town, route, or map of the position
     */
    const std::string& getLocationName(const component::Position& pos) const;

protected:
    std::string nameField;
    std::string loadScriptField;
    std::string unloadScriptField;
    std::string playlistField;
    Weather::Type weatherField;
    std::vector<LayerSet> levels;
    std::string tilesetField;
    std::unordered_map<std::uint16_t, Spawn> spawns;
    std::vector<CharacterSpawn> characterField;
    std::vector<Item> itemsField;
    std::vector<Event> eventsField;
    LightingSystem lighting;
    std::vector<CatchRegion> catchRegionsField;
    bl::container::Vector2D<LevelTransition> transitionField;
    std::vector<Town> towns;
    bl::container::Vector2D<std::uint8_t> townTiles;

    system::Systems* systems;
    Town defaultTown;
    Town* currentTown;
    sf::Vector2i size;
    bl::resource::Resource<Tileset>::Ref tileset;
    Weather weather;
    std::unique_ptr<bl::script::Script> onEnterScript;
    std::unique_ptr<bl::script::Script> onExitScript;
    bl::container::Grid<const Event*> eventRegions;
    bool isWorldMap;
    bl::audio::AudioSystem::Handle playlistHandle;

    bool activated; // for weather continuity
    mutable sf::IntRect renderRange;
    mutable sf::RectangleShape cover;

    void clear();
    void triggerAnimation(const component::Position& position);
    void refreshRenderRange(const sf::View& view) const;
    Town* getTown(const sf::Vector2i& pos);
    void enterTown(Town* town);

    static std::vector<Town> flymapTowns;
    static void loadFlymapTowns();

    friend class loaders::LegacyMapLoader;
    friend class bl::serial::SerializableObject<Map>;
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::Map> : public SerializableObjectBase {
    using M = core::map::Map;

    SerializableField<1, M, std::string> nameField;
    SerializableField<2, M, std::string> loadScriptField;
    SerializableField<3, M, std::string> unloadScriptField;
    SerializableField<4, M, std::string> playlistField;
    SerializableField<5, M, core::map::Weather::Type> weatherField;
    SerializableField<6, M, std::vector<core::map::LayerSet>> levels;
    SerializableField<7, M, std::string> tilesetField;
    SerializableField<8, M, std::unordered_map<std::uint16_t, core::map::Spawn>> spawnField;
    SerializableField<9, M, std::vector<core::map::CharacterSpawn>> characterField;
    SerializableField<10, M, std::vector<core::map::Item>> itemsField;
    SerializableField<11, M, std::vector<core::map::Event>> eventsField;
    SerializableField<12, M, core::map::LightingSystem, false> lighting; // 13 was catch zones
    SerializableField<14, M, bl::container::Vector2D<core::map::LevelTransition>> transitionField;
    SerializableField<15, M, std::vector<core::map::CatchRegion>> catchRegionsField;
    SerializableField<16, M, std::vector<core::map::Town>> townsField;
    SerializableField<17, M, bl::container::Vector2D<std::uint8_t>> townTiles;

    SerializableObject()
    : nameField("name", *this, &M::nameField, SerializableFieldBase::Required{})
    , loadScriptField("loadScript", *this, &M::loadScriptField, SerializableFieldBase::Required{})
    , unloadScriptField("unloadScript", *this, &M::unloadScriptField,
                        SerializableFieldBase::Required{})
    , playlistField("playlist", *this, &M::playlistField, SerializableFieldBase::Required{})
    , weatherField("weather", *this, &M::weatherField, SerializableFieldBase::Required{})
    , levels("levels", *this, &M::levels, SerializableFieldBase::Required{})
    , tilesetField("tileset", *this, &M::tilesetField, SerializableFieldBase::Required{})
    , spawnField("spawns", *this, &M::spawns, SerializableFieldBase::Required{})
    , characterField("characters", *this, &M::characterField, SerializableFieldBase::Required{})
    , itemsField("items", *this, &M::itemsField, SerializableFieldBase::Required{})
    , eventsField("events", *this, &M::eventsField, SerializableFieldBase::Required{})
    , lighting("lighting", *this, &M::lighting, SerializableFieldBase::Required{})
    , transitionField("transitions", *this, &M::transitionField, SerializableFieldBase::Required{})
    , catchRegionsField("catchZones", *this, &M::catchRegionsField,
                        SerializableFieldBase::Required{})
    , townsField("towns", *this, &M::towns, SerializableFieldBase::Required{})
    , townTiles("townTiles", *this, &M::townTiles, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
