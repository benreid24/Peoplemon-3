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
        systems.player().inputSystem().addListener(*this);
        started = true;
    }
}

void PlayerControlled::stop() {
    started = false;
    systems.player().inputSystem().removeListener(*this);
}

void PlayerControlled::process(Command c) {
    switch (c) {
    case Command::Pause:
        systems.engine().eventBus().dispatch<event::StateChange>({event::StateChange::GamePaused});
        break;

    case Command::Back:
        systems.engine().eventBus().dispatch<event::StateChange>({event::StateChange::BackPressed});
        break;

    default:
        controllable.get().processControl(c);
        break;
    }
}

} // namespace component
} // namespace core
