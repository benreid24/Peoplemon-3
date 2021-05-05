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

} // namespace component
} // namespace core

#endif
