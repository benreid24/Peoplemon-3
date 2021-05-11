#include <Core/Components/SpinBehavior.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
SpinBehavior::SpinBehavior(const bl::entity::Registry::ComponentHandle<Position>& position,
                           const bl::entity::Registry::ComponentHandle<Controllable>& controllable,
                           file::Behavior::Spinning::Direction dir)
: dir(dir)
, position(position)
, controllable(controllable)
, standTime(0.f) {}

void SpinBehavior::update(float dt) {
    standTime += dt;
    if (standTime >= Properties::CharacterSpinPeriod()) {
        standTime = 0.f;
        switch (dir) {
        case file::Behavior::Spinning::Clockwise: {
            const int ndir    = (static_cast<int>(position.get().direction) + 1) % 4;
            const Command cmd = static_cast<Command>(ndir + 1);
            controllable.get().processControl(cmd);
        } break;

        case file::Behavior::Spinning::Counterclockwise: {
            const int ndir    = (static_cast<int>(position.get().direction) - 1) % 4;
            const Command cmd = static_cast<Command>(ndir + 1);
            controllable.get().processControl(cmd);
        } break;

        case file::Behavior::Spinning::Random: {
            Direction ndir = static_cast<Direction>(bl::util::Random::get<int>(0, 3));
            while (ndir == position.get().direction) {
                ndir = static_cast<Direction>(bl::util::Random::get<int>(0, 3));
            }
            const Command cmd = static_cast<Command>(static_cast<int>(dir + 1));
            controllable.get().processControl(cmd);
        } break;

        default:
            break;
        }
    }
}

} // namespace component
} // namespace core
