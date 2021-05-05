#include <Core/Components/PlayerControlled.hpp>

namespace core
{
namespace component
{
PlayerControlled::PlayerControlled(
    const bl::entity::Registry::ComponentHandle<component::Controllable>& controllable,
    player::Input& input)
: controllable(controllable)
, input(input)
, started(false) {}

void PlayerControlled::start() {
    if (!started) {
        input.addListener(*this);
        started = true;
    }
}

void PlayerControlled::stop() {
    started = false;
    input.removeListener(*this);
}

void PlayerControlled::process(Command c) { controllable.get().processControl(c); }

} // namespace component
} // namespace core
