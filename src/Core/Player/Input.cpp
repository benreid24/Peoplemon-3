#include <Core/Player/Input.hpp>

#include <Core/Properties.hpp>

namespace core
{
namespace player
{
Input::Input() {
    using namespace input;
    controls[Controls::Up]       = sf::Keyboard::W;
    controls[Controls::Right]    = sf::Keyboard::D;
    controls[Controls::Down]     = sf::Keyboard::S;
    controls[Controls::Left]     = sf::Keyboard::A;
    controls[Controls::Run]      = sf::Keyboard::LShift;
    controls[Controls::Interact] = sf::Keyboard::Space;
    controls[Controls::Pause]    = sf::Keyboard::Return;
    controls[Controls::Back]     = sf::Keyboard::LControl;
}

void Input::addListener(input::Listener& l) { listeners.push_back(&l); }

void Input::removeListener(input::Listener& l) {
    for (int i = listeners.size() - 1; i >= 0; --i) {
        if (listeners[i] == &l) { listeners.erase(listeners.begin() + i); }
    }
}

void Input::update() {
    if (moving) {
        if (running) {
            switch (moveDir) {
            case component::Direction::Up:
                dispatch(component::Control::SprintUp);
                break;

            case component::Direction::Right:
                dispatch(component::Control::SprintRight);
                break;

            case component::Direction::Down:
                dispatch(component::Control::SprintDown);
                break;

            case component::Direction::Left:
                dispatch(component::Control::SprintLeft);
                break;

            default:
                BL_LOG_WARN << "Unknown direction: " << static_cast<int>(moveDir);
                break;
            }
        }
        else {
            switch (moveDir) {
            case component::Direction::Up:
                dispatch(component::Control::MoveUp);
                break;

            case component::Direction::Right:
                dispatch(component::Control::MoveRight);
                break;

            case component::Direction::Down:
                dispatch(component::Control::MoveDown);
                break;

            case component::Direction::Left:
                dispatch(component::Control::MoveLeft);
                break;

            default:
                BL_LOG_WARN << "Unknown direction: " << static_cast<int>(moveDir);
                break;
            }
        }
    }
}

void Input::dispatch(component::Control c) {
    if (!listeners.empty()) listeners.back()->process(c);
}

void Input::observe(const sf::Event& event) {
    using namespace input;
    if (controls[Controls::Up].active(event)) {
        moving = true;
        moveDir = component::Direction::Up;
    }
    else if (controls[Controls::Right].active(event)) {
        moving = true;
        moveDir = component::Direction::Right;
    }
}

} // namespace player
} // namespace core
