#include <Core/Player/Input/MenuDriver.hpp>

namespace core
{
namespace player
{
namespace input
{
MenuDriver::MenuDriver()
: Listener()
, menu(nullptr)
, back(false) {}

void MenuDriver::drive(bl::menu::Menu& m) { menu = &m; }

bool MenuDriver::backPressed() {
    const bool b = back;
    back         = false;
    return b;
}

void MenuDriver::process(component::Command cmd) {
    if (!menu) return;

    switch (cmd) {
    case component::Command::MoveDown:
        menu->processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Bottom)});
        break;

    case component::Command::MoveRight:
        menu->processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Right)});
        break;

    case component::Command::MoveUp:
        menu->processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Top)});
        break;

    case component::Command::MoveLeft:
        menu->processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Left)});
        break;

    case component::Command::Interact:
        menu->processEvent({bl::menu::Event::ActivateEvent()});
        break;

    case component::Command::Back:
        back = true;
        break;

    default:
        break;
    }
}

} // namespace input
} // namespace player
} // namespace core
