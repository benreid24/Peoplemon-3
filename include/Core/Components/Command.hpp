#ifndef CORE_COMPONENTS_COMMAND_HPP
#define CORE_COMPONENTS_COMMAND_HPP

#include <Core/Components/Direction.hpp>
#include <cstdint>

namespace core
{
namespace component
{
/**
 * @brief Basic enum representing a command that can be issued to a controllable component
 *
 * @ingroup Components
 *
 */
enum struct Command : std::uint8_t {
    None        = 0,
    MoveUp      = 1,
    MoveRight   = 2,
    MoveDown    = 3,
    MoveLeft    = 4,
    SprintUp    = 5,
    SprintRight = 6,
    SprintDown  = 7,
    SprintLeft  = 8,
    Interact    = 9,
    Pause       = 10,
    Back        = 11
};

/**
 * @brief Returns the command to move in the given direction
 *
 * @param dir The direction to move in
 * @param run True to run, false to walk
 * @return Command The corresponding command
 *
 * @ingroup Components
 */
Command moveCommand(Direction dir, bool run = false);

} // namespace component
} // namespace core

#endif
