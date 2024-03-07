#ifndef CORE_SYSTEMS_WORLD_HPP
#define CORE_SYSTEMS_WORLD_HPP

#include <BLIB/Engine/System.hpp>
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
 */
class World
: public bl::engine::System
, public bl::event::Listener<event::GameSaveInitializing, event::GameSaveLoaded> {
public:
    /**
     * @brief Creates the world system
     *
     * @param systems A reference to the parent Systems object
     */
    World(Systems& systems);

    /**
     * @brief Destroys the system
     */
    virtual ~World() = default;

    /**
     * @brief Switches the current map to the map in the given file
     *
     * @param newMap Filename of new map, or "LastMap" for previous map
     * @param spawnId The id to spawn at. Ignored if "LastMap" is specified
     * @return True if the new map could be loaded and switched to, false on error
     */
    bool switchMaps(const std::string& newMap, int spawnId);

    /**
     * @brief Switches to the new map and resets the last map to empty
     *
     * @param newMap The map to whiteout into
     * @param spawnId Where to spawn
     */
    void whiteout(const std::string& newMap, int spawnId);

    /**
     * @brief Returns a reference to the active map
     */
    map::Map& activeMap();

    /**
     * @brief Returns a const reference to the active map
     */
    const map::Map& activeMap() const;

    /**
     * @brief Adds saved world data to the save file
     */
    virtual void observe(const event::GameSaveInitializing& save) override;

    /**
     * @brief Initializes world state from the loading game save
     */
    virtual void observe(const event::GameSaveLoaded& load) override;

    /**
     * @brief Sets the respawn point to the given spawn in the current map
     *
     * @param spawn The spawn to respawn at
     */
    void setWhiteoutMap(unsigned int spawn);

private:
    Systems& owner;
    bl::resource::Ref<map::Map> currentMap;
    bl::resource::Ref<map::Map> previousMap;

    std::string currentMapFile;
    std::string prevMapFile;
    bl::tmap::Position playerPos;
    bl::tmap::Position prevPlayerPos;

    virtual void init(bl::engine::Engine&) override;
    virtual void update(std::mutex&, float dt, float, float, float) override;
    virtual void earlyCleanup() override;
};

} // namespace system
} // namespace core

#endif
