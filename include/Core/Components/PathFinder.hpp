#ifndef CORE_COMPONENTS_PATHFINDER_HPP
#define CORE_COMPONENTS_PATHFINDER_HPP

#include <BLIB/Tilemap/Position.hpp>
#include <vector>

namespace core
{
namespace component
{
/**
 * @brief Component that dynamically navigates an entity to a specific tile. The entities
 *        Controllable component is locked so that only path finding controls it
 *
 * @ingroup Components
 *
 */
struct PathFinder {
    /**
     * @brief Construct a new Path Finder object
     *
     * @param dest The destination to navigate to
     */
    PathFinder(const bl::tmap::Position& dest)
    : destination(dest)
    , step(0) {}

    bl::tmap::Position destination;
    std::vector<bl::tmap::Position> path;
    unsigned int step;
};

} // namespace component
} // namespace core

#endif
