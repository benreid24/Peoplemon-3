#include <Core/Components/SpinBehavior.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
SpinBehavior::SpinBehavior(file::Behavior::Spinning::Direction dir)
: dir(dir)
, standTime(0.f) {}

void SpinBehavior::update(Position& position, Controllable& controller, float dt) {
    standTime += dt;
    if (standTime >= Properties::CharacterSpinPeriod()) {
        standTime = 0.f;
        switch (dir) {
        case file::Behavior::Spinning::Clockwise: {
            controller.processControl(moveCommand(nextClockwiseDirection(position.direction)));
        } break;

        case file::Behavior::Spinning::Counterclockwise: {
            controller.processControl(
                moveCommand(nextCounterClockwiseDirection(position.direction)));
        } break;

        case file::Behavior::Spinning::Random: {
            Direction ndir = static_cast<Direction>(bl::util::Random::get<int>(0, 3));
            while (ndir == position.direction) {
                ndir = static_cast<Direction>(bl::util::Random::get<int>(0, 3));
            }
            controller.processControl(moveCommand(ndir));
        } break;

        default:
            break;
        }
    }
}

} // namespace component
} // namespace core
