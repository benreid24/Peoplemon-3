#include <Core/Input/Control.hpp>

namespace core
{
namespace input
{
void configureInputSystem(bl::input::InputSystem& system) {
    // register controls
    system.setControlCount(Control::Count);
    system.configureTriggerControl(Control::MoveUp);
    system.configureTriggerControl(Control::MoveRight);
    system.configureTriggerControl(Control::MoveDown);
    system.configureTriggerControl(Control::MoveLeft);
    system.configureTriggerControl(Control::Sprint);
    system.configureTriggerControl(Control::Interact);
    system.configureTriggerControl(Control::Pause);
    system.configureTriggerControl(Control::Back);

    // creator the actor. Peoplemon is only 1 player locally
    bl::input::Actor& user = system.addActor();

    // set keyboard and mouse defaults. no defaults for controller
    user.getKBMTriggerControl(Control::MoveUp).triggerOnKey(sf::Keyboard::W);
    user.getKBMTriggerControl(Control::MoveRight).triggerOnKey(sf::Keyboard::D);
    user.getKBMTriggerControl(Control::MoveDown).triggerOnKey(sf::Keyboard::S);
    user.getKBMTriggerControl(Control::MoveLeft).triggerOnKey(sf::Keyboard::A);
    user.getKBMTriggerControl(Control::Sprint).triggerOnKey(sf::Keyboard::LShift);
    user.getKBMTriggerControl(Control::Interact).triggerOnKey(sf::Keyboard::Space);
    user.getKBMTriggerControl(Control::Pause).triggerOnKey(sf::Keyboard::Enter);
    user.getKBMTriggerControl(Control::Back).triggerOnKey(sf::Keyboard::LControl);

    // load user preferences from config
    system.loadFromConfig();
}

EntityControl fromDirection(bl::tmap::Direction dir) {
    switch (dir) {
    case bl::tmap::Direction::Up:
        return Control::MoveUp;
    case bl::tmap::Direction::Right:
        return Control::MoveRight;
    case bl::tmap::Direction::Down:
        return Control::MoveDown;
    case bl::tmap::Direction::Left:
    default:
        return Control::MoveLeft;
    }
}

} // namespace input
} // namespace core
