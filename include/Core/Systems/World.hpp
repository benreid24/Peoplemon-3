#ifndef CORE_SYSTEMS_WORLD_HPP
#define CORE_SYSTEMS_WORLD_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Serialization/JSON.hpp>
#include <Core/Events/GameSave.hpp>
#include <Core/Maps/Map.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief System for managing the current map and previous maps
 *
 * @ingroup Systems
 *
 */
class World : public bl::event::Listener<event::GameSaving, event::GameLoading> {
public:
    /**
     * @brief Creates the world system
     *
     * @param systems A reference to the parent Systems object
     */
    World(Systems& systems);

    /**
     * @brief Custom cleanup code to prevent dangling pointers in event subscribers
     *
     */
    ~World();

    /**
     * @brief Subscribes the system to the engine event bus
     *
     */
    void init();

    /**
     * @brief Switches the current map to the map in the given file
     *
     * @param newMap Filename of new map, or "LastMap" for previous map
     * @param spawnId The id to spawn at. Ignored if "LastMap" is specified
     * @return True if the new map could be loaded and switched to, false on error
     */
    bool switchMaps(const std::string& newMap, int spawnId);

    /**
     * @brief Returns a reference to the active map
     *
     */
    map::Map& activeMap();

    /**
     * @brief Returns a const reference to the active map
     *
     */
    const map::Map& activeMap() const;

    /**
     * @brief Updates the current map
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    void update(float dt);

    /**
     * @brief Adds saved world data to the save file
     *
     */
    virtual void observe(const event::GameSaving& save) override;

    /**
     * @brief Initializes world state from the loading game save
     *
     */
    virtual void observe(const event::GameLoading& load) override;

private:
    Systems& owner;
    bl::resource::Resource<map::Map>::Ref currentMap;
    bl::resource::Resource<map::Map>::Ref previousMap;

    std::string currentMapFile;
    std::string prevMapFile;
    component::Position playerPos;
    component::Position prevPlayerPos;

    friend class bl::serial::json::SerializableObject<World>;
};

} // namespace system
} // namespace core

namespace bl
{
namespace serial
{
namespace json
{
template<>
struct SerializableObject<core::system::World> : SerializableObjectBase {
    using World = core::system::World;
    using Pos   = core::component::Position;

    SerializableField<World, std::string> currentMap;
    SerializableField<World, std::string> prevMap;
    SerializableField<World, Pos> playerPos;
    SerializableField<World, Pos> prevPlayerPos;

    SerializableObject()
    : currentMap("current", *this, &World::currentMapFile)
    , prevMap("previous", *this, &World::prevMapFile)
    , playerPos("position", *this, &World::playerPos)
    , prevPlayerPos("prevPos", *this, &World::prevPlayerPos) {}
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
