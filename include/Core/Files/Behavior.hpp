#ifndef CORE_FILES_BEHAVIOR_HPP
#define CORE_FILES_BEHAVIOR_HPP

#include <cstdint>

namespace core
{
namespace file
{
/**
 * @brief Set of behaviors for NPCs and trainers
 *
 * @ingroup Files
 *
 */
enum struct Behavior : std::uint8_t {
    ///
    StandStill = 0,

    ///
    Spinning = 1,

    ///
    FollowingPath = 2,

    ///
    Wandering = 3
};

} // namespace file
} // namespace core

#endif
