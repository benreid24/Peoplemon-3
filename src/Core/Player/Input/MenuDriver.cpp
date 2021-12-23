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
, back(false)
, pause(false)
, debounce(0.3f)
, lastInput(0.f) {}

void MenuDriver::drive(bl::menu::Menu* m) { menu = m; }

bool MenuDriver::backPressed() {
    const bool b = back;
    back         = false;
    return b;
}

bool MenuDriver::pausePressed() {
    const bool p = pause;
    pause        = false;
    return p;
}

void MenuDriver::process(component::Command cmd) {
    if (timer.getElapsedTime().asSeconds() - lastInput < debounce) return;
    lastInput = timer.getElapsedTime().asSeconds();
    processImmediate(cmd);
}

void MenuDriver::processImmediate(component::Command cmd) {
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

    case component::Command::Pause:
        pause = true;
        break;

    default:
        break;
    }
}

void MenuDriver::setInputDelay(float d) { debounce = d; }

} // namespace input
} // namespace player
} // namespace core
