#ifndef CORE_ENTITIES_DIRECTION_HPP
#define CORE_ENTITIES_DIRECTION_HPP

#include <cstdint>

/**
 * @addtogroup Components
 * @ingroup Core
 * @brief Collection of components for in game entities
 *
 */

namespace core
{
/// Collection of components for in game entities
namespace component
{
/**
 * @brief Which direction an entity is facing
 *
 * @ingroup Components
 *
 */
enum struct Direction : std::uint8_t { Up = 0, Right = 1, Down = 2, Left = 3 };

/**
 * @brief Returns the Direction that is opposite of the given direction
 *
 * @param dir The direction to get the opposite of
 * @return Direction The direction facing opposite of dir
 *
 * @ingroup Components
 */
Direction oppositeDirection(Direction dir);

} // namespace component
} // namespace core

#endif
