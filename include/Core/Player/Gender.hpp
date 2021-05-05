#ifndef CORE_PLAYER_GENDER_HPP
#define CORE_PLAYER_GENDER_HPP

#include <cstdint>

/**
 * @addtogroup Player
 * @ingroup Core
 * @brief Collection of classes for managing player state and functionality
 *
 */

namespace core
{
/// Collection of classes for managing player state and functionality
namespace player
{
/**
 * @brief Possible genders for the player
 *
 * @ingroup Player
 *
 */
enum struct Gender : std::uint8_t { Boy = 1, Girl = 2 };

} // namespace player
} // namespace core

#endif
