#include <Core/Components/PlayerControlled.hpp>

#include <Core/Events/StateChange.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace component
{
PlayerControlled::PlayerControlled(system::Systems& s, Controllable& controllable)
: systems(s)
, controllable(controllable)
, started(false) {}

void PlayerControlled::start() {
    if (!started) {
        systems.engine().inputSystem().getActor().addListener(*this);
        started = true;
    }
}

void PlayerControlled::stop() {
    started = false;
    systems.engine().inputSystem().getActor().removeListener(*this);
}

bool PlayerControlled::observe(const bl::input::Actor& actor, unsigned int ctrl,
                               bl::input::DispatchType, bool fromEvent) {
    if (controllable.isLocked()) return true;
    if (ctrl == input::Control::Interact && !fromEvent) return true;

    switch (ctrl) {
    case input::Control::Pause:
        if (fromEvent) {
            bl::event::Dispatcher::dispatch<event::StateChange>(
                {event::StateChange::GamePaused});
        }
        break;

    case input::Control::Back:
        break;

    default:
        controllable.processControl(static_cast<input::EntityControl>(ctrl),
                                    actor.controlActive(input::Control::Sprint));
        break;
    }
    return true;
}

} // namespace component
} // namespace core
