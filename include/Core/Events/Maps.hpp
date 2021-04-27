#ifndef CORE_EVENTS_MAPS_HPP
#define CORE_EVENTS_MAPS_HPP

#include <string>

namespace core
{
namespace event
{
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
     * @param map The name of the map that was entered
     */
    MapEntered(const std::string& map)
    : mapName(map) {}

    /// The name of the map that was entered
    const std::string mapName;
};

/**
 * @brief Fired when a map is exited
 *
 * @ingroup Events
 *
 */
struct MapExited {
    /**
     * @brief Construct a new Map Exited object
     *
     * @param map The map that was exited
     */
    MapExited(const std::string& map)
    : mapName(map) {}

    /// The map that was exited
    const std::string mapName;
};

} // namespace event
} // namespace core

#endif
