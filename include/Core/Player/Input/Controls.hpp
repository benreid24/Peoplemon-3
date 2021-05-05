#ifndef CORE_PLAYER_INPUT_CONTROLS_HPP
#define CORE_PLAYER_INPUT_CONTROLS_HPP

#include <cstdint>

namespace core
{
namespace player
{
namespace input
{
/// Gives scoped access to unscoped Controls enum
namespace Controls
{
/**
 * @brief The different player controls that need to be mapped and processed
 *
 * @ingroup Player
 *
 */
enum Type : std::uint8_t {
    /// Walk or run up (hold)
    Up = 0,

    /// Walk or run right (hold)
    Right,

    /// Walk or run down (hold)
    Down,

    /// Walk or run left (hold)
    Left,

    /// Run when moving (hold)
    Run,

    /// Pause the game (press)
    Pause,

    /// Navigate back or open special menu (press)
    Back,

    /// Interact or select (press)
    Interact,

    /// The number of controls
    COUNT
};

} // namespace Controls
} // namespace input
} // namespace player
} // namespace core

#endif
