#ifndef CORE_EVENTS_MAPS_HPP
#define CORE_EVENTS_MAPS_HPP

#include <string>

namespace core
{
namespace map
{
class Map;
}

namespace event
{
/**
 * @brief Fired right before a map is entered. Use this event for initializing data structures that
 *        depend on map size that need to be initialized before entities are spawned
 *
 * @ingroup Events
 *
 */
struct MapSwitch {
    /**
     * @brief Construct a new MapSwitch event
     *
     * @param map The map that is going to be entered
     */
    MapSwitch(const map::Map& map)
    : map(map) {}

    /// The map that is going to be entered
    const map::Map& map;
};

/**
 * @brief Fired when a map is entered
 *
 * @ingroup Events
 *
 */
struct MapEntered {
    /**
     * @brief Construct a new Map Entered object
     *
     * @param map The map that was entered
     */
    MapEntered(const map::Map& map)
    : map(map) {}

    /// A reference to the map that was just entered
    const map::Map& map;
};

/**
 * @brief Fired when a map is exited
 *
 * @ingroup Events
 *
 */
struct MapExited {
    /**
     * @brief Construct a new MapExited event
     *
     * @param map The map that was exited
     */
    MapExited(const map::Map& map)
    : map(map) {}

    /// A reference to the map that was just exited
    const map::Map& map;
};

/**
 * @brief Fired when the map should be switched to something else
 *
 * @ingroup Events
 *
 */
struct SwitchMapTriggered {
    /**
     * @brief Construct a new Switch Map Triggered event
     *
     * @param newMap The file of the new map
     * @param spawn The spawn to spawn the player at
     */
    SwitchMapTriggered(const std::string& newMap, int spawn)
    : newMap(newMap)
    , spawn(spawn) {}

    /// The new map to switch to
    const std::string newMap;

    /// The spawn to start the player at
    const int spawn;
};

} // namespace event
} // namespace core

#endif
