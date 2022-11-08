#include <Core/Components/PlayerControlled.hpp>

#include <Core/Events/StateChange.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace component
{
PlayerControlled::PlayerControlled(
    system::Systems& s,
    const bl::entity::Registry::ComponentHandle<component::Controllable>& controllable)
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
                               bl::input::DispatchType, bool) {
    if (controllable.get().isLocked()) return false;

    switch (ctrl) {
    case input::Control::Pause:
        systems.engine().eventBus().dispatch<event::StateChange>({event::StateChange::GamePaused});
        return true;

    case input::Control::Back:
        systems.engine().eventBus().dispatch<event::StateChange>({event::StateChange::BackPressed});
        return true;

    default:
        return controllable.get().processControl(static_cast<input::EntityControl>(ctrl),
                                                 actor.controlActive(input::Control::Sprint));
    }
}

} // namespace component
} // namespace core
