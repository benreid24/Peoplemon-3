#ifndef CORE_MAPS_LEVELTRANSITION_HPP
#define CORE_MAPS_LEVELTRANSITION_HPP

#include <cstdint>

namespace core
{
namespace map
{
/**
 * @brief Represents a level transition in a map. Level transitions are applied when an entity moves
 *        either onto or out of a tile containing a transition. When the transition is for going up
 *        the change occurs when the tile is left. When it is going down it is applied when the tile
 *        is entered
 *
 * @ingroup Maps
 *
 */
enum struct LevelTransition : std::uint8_t {
    /// No level transition, entity stays on same level
    None = 0,

    /// Entities moving horizontally move up when going right and down when going left
    HorizontalRightUp = 1,

    /// Entities moving horizontally move down when going right and up when going left
    HorizontalRightDown = 2,

    /// Entities moving vertically move up when going north and down when going south
    VerticalTopUp = 3,

    /// Entities moving vertically move down when going north and up when going south
    VerticalTopDown = 4
};

} // namespace map
} // namespace core

#endif
