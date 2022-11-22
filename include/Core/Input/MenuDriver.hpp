#ifndef CORE_INPUT_MENUDRIVER_HPP
#define CORE_INPUT_MENUDRIVER_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Input/Control.hpp>

namespace core
{
namespace input
{
/**
 * @brief Helper typedef for Peoplemon specific menu driving
 *
 * @ingroup PlayerInput
 *
 */
using MenuDriver = bl::menu::TriggerDriver<Control::MoveUp, Control::MoveRight, Control::MoveDown,
                                           Control::MoveLeft, Control::Interact>;

} // namespace input
} // namespace core

#endif
