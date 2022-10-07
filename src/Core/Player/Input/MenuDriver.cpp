#include <Core/Player/Input/MenuDriver.hpp>

namespace core
{
namespace player
{
namespace input
{
namespace
{
sf::Clock timer;
}

MenuDriver::MenuDriver()
: Listener()
, menu(nullptr)
, debounce(0.3f)
, lastInput(0.f)
, prevInput(component::Command::None) {}

void MenuDriver::drive(bl::menu::Menu* m) { menu = m; }

component::Command MenuDriver::mostRecentInput() {
    const component::Command cmd = prevInput;
    prevInput                    = component::Command::None;
    return cmd;
}

void MenuDriver::process(component::Command cmd) {
    if (timer.getElapsedTime().asSeconds() - lastInput < debounce) return;
    lastInput = timer.getElapsedTime().asSeconds();
    processImmediate(cmd);
}

void MenuDriver::processImmediate(component::Command cmd) {
    prevInput = cmd;
    if (!menu) return;
    MenuDriver::sendToMenu(*menu, cmd);
}

void MenuDriver::setInputDelay(float d) { debounce = d; }

void MenuDriver::sendToMenu(bl::menu::Menu& menu, component::Command cmd) {
    switch (cmd) {
    case component::Command::MoveDown:
        menu.processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Bottom)});
        break;

    case component::Command::MoveRight:
        menu.processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Right)});
        break;

    case component::Command::MoveUp:
        menu.processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Top)});
        break;

    case component::Command::MoveLeft:
        menu.processEvent({bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Left)});
        break;

    case component::Command::Interact:
        menu.processEvent({bl::menu::Event::ActivateEvent()});
        break;

    case component::Command::Back:
    case component::Command::Pause:
    default:
        break;
    }
}

} // namespace input
} // namespace player
} // namespace core
