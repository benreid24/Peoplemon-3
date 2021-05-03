#ifndef CORE_COMPONENTS_CONTROL_HPP
#define CORE_COMPONENTS_CONTROL_HPP

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
enum struct Control : std::uint8_t {
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

} // namespace component
} // namespace core

#endif
