#ifndef CORE_INPUT_CONTROL_HPP
#define CORE_INPUT_CONTROL_HPP

#include <BLIB/Input.hpp>
#include <BLIB/Tilemap/Direction.hpp>

/**
 * @addtogroup PlayerInput
 * @ingroup Core
 * @brief Peoplemon specific input functionality around the BLIB input system
 *
 */

namespace core
{
/// @brief Peoplemon specific input functionality around the BLIB input system
namespace input
{
/// @brief Wrapper around the enum for player input ids
namespace Control
{
/**
 * @brief Contains the various inputs a player or AI can use to control the game or an NPC
 *
 * @ingroup PlayerInput
 *
 */
enum EntityControl : unsigned int {
    MoveUp    = 0,
    MoveRight = 1,
    MoveDown  = 2,
    MoveLeft  = 3,
    Sprint    = 4,
    Interact  = 5,
    Pause     = 6,
    Back      = 7,
    Count,
    None = Count
};

} // namespace Control

/// @brief Helper typedef to avoid too much casting boilerplate
using EntityControl = std::underlying_type_t<Control::EntityControl>;

/**
 * @brief Configures the input system for the set of peoplemon inputs
 *
 * @param system The input system to configure
 */
void configureInputSystem(bl::input::InputSystem& system);

/**
 * @brief Helper method to convert a move direction to a control
 *
 * @param direction The direction to convert to a control
 * @return The control to move in the given direction
 */
EntityControl fromDirection(bl::tmap::Direction direction);

} // namespace input
} // namespace core

#endif
