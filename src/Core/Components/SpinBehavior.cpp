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

void SpinBehavior::update(bl::tmap::Position& position, Controllable& controller, float dt) {
    standTime += dt;
    if (standTime >= Properties::CharacterSpinPeriod()) {
        standTime -= Properties::CharacterSpinPeriod();
        switch (dir) {
        case file::Behavior::Spinning::Clockwise: {
            controller.processControl(
                input::fromDirection(nextClockwiseDirection(position.direction)));
        } break;

        case file::Behavior::Spinning::Counterclockwise: {
            controller.processControl(
                input::fromDirection(nextCounterClockwiseDirection(position.direction)));
        } break;

        case file::Behavior::Spinning::Random: {
            bl::tmap::Direction ndir =
                static_cast<bl::tmap::Direction>(bl::util::Random::get<int>(0, 3));
            while (ndir == position.direction) {
                ndir = static_cast<bl::tmap::Direction>(bl::util::Random::get<int>(0, 3));
            }
            controller.processControl(input::fromDirection(ndir));
        } break;

        default:
            break;
        }
    }
}

} // namespace component
} // namespace core
