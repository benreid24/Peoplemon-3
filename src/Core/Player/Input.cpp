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
    controls[Controls::Pause]    = sf::Keyboard::Enter;
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
                dispatch(component::Command::SprintUp);
                break;

            case component::Direction::Right:
                dispatch(component::Command::SprintRight);
                break;

            case component::Direction::Down:
                dispatch(component::Command::SprintDown);
                break;

            case component::Direction::Left:
                dispatch(component::Command::SprintLeft);
                break;

            default:
                BL_LOG_WARN << "Unknown direction: " << static_cast<int>(moveDir);
                break;
            }
        }
        else {
            switch (moveDir) {
            case component::Direction::Up:
                dispatch(component::Command::MoveUp);
                break;

            case component::Direction::Right:
                dispatch(component::Command::MoveRight);
                break;

            case component::Direction::Down:
                dispatch(component::Command::MoveDown);
                break;

            case component::Direction::Left:
                dispatch(component::Command::MoveLeft);
                break;

            default:
                BL_LOG_WARN << "Unknown direction: " << static_cast<int>(moveDir);
                break;
            }
        }
    }
}

void Input::dispatch(component::Command c) {
    if (!listeners.empty()) listeners.back()->process(c);
}

void Input::observe(const sf::Event& event) {
    using namespace input;

    const auto checkMove = [this, &event](Controls::Type c) {
        if (this->controls[c].matches(event)) {
            const component::Direction d = static_cast<component::Direction>(c);
            if (this->controls[c].activated(event)) {
                this->moving  = true;
                this->moveDir = d;
            }
            else if (this->controls[c].deactivated(event) && this->moveDir == d) {
                this->moving = false;
            }
        }
    };

    checkMove(Controls::Up);
    checkMove(Controls::Right);
    checkMove(Controls::Down);
    checkMove(Controls::Up);

    if (controls[Controls::Run].matches(event)) {
        if (controls[Controls::Run].activated(event)) { running = true; }
        else if (controls[Controls::Run].deactivated(event)) {
            running = false;
        }
    }
    else if (controls[Controls::Interact].activated(event)) {
        dispatch(component::Command::Interact);
    }
    else if (controls[Controls::Pause].activated(event)) {
        dispatch(component::Command::Pause);
    }
    else if (controls[Controls::Back].activated(event)) {
        dispatch(component::Command::Back);
    }
}

} // namespace player
} // namespace core
