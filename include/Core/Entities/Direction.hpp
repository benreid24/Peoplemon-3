#ifndef CORE_ENTITIES_DIRECTION_HPP
#define CORE_ENTITIES_DIRECTION_HPP

#include <cstdint>

/**
 * @addtogroup Entities
 * @ingroup Core
 * @brief Collection of components and systems related to in game entities
 *
 */

namespace core
{
/// Collection of components and systems related to in game entities
namespace entity
{
/**
 * @brief Which direction an entity is facing
 *
 * @ingroup Entities
 *
 */
enum struct Direction : std::uint8_t { Up = 0, Right = 1, Down = 2, Left = 3 };

} // namespace entity
} // namespace core

#endif
