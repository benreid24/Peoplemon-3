#include <Core/Components/WanderBehavior.hpp>

#include <BLIB/Util/Random.hpp>

namespace core
{
namespace component
{
WanderBehavior::WanderBehavior(unsigned int radius)
: radius(radius)
, state(Walking)
, stateTime(0.f)
, transitionTime(0.f) {
    switchState();
}

void WanderBehavior::update(Position& position, Controllable& controller, float dt) {
    stateTime += dt;
    if (stateTime >= transitionTime) { switchState(); }

    switch (state) {
    case Walking: {
        if (controller.processControl(moveCommand(data.walking.dir)))
            data.walking.steps += 1;
        else if (!position.moving())
            data.walking.steps = 10; // ensures new direction

        if (bl::util::Random::get<unsigned int>(1, 4) >= data.walking.steps) {
            //
        }
    } break;
    case Standing:
        data.standing.update(position, controller);
        break;
    case Spinning:
        data.spinning.update(position, controller, dt);
        break;
    default:
        break;
    }
}

void WanderBehavior::switchState() {
    switch (state) {
    case Walking:
        data.walking.~WalkData();
        break;
    case Standing:
        data.standing.~StandingBehavior();
        break;
    case Spinning:
        data.spinning.~SpinBehavior();
        break;
    default:
        break;
    }

    state          = static_cast<State>(bl::util::Random::get<int>(0, NUM_STATES));
    stateTime      = 0.f;
    transitionTime = bl::util::Random::get<float>(2.5f, 15.f);

    switch (state) {
    case Walking:
        new (&data.walking) WalkData();
        data.walking.dir = static_cast<Direction>(bl::util::Random::get<unsigned int>(0, 3));
        break;
    case Standing:
        new (&data.standing)
            StandingBehavior(static_cast<Direction>(bl::util::Random::get<unsigned int>(0, 3)));
        break;
    case Spinning:
        new (&data.spinning) SpinBehavior(static_cast<file::Behavior::Spinning::Direction>(
            bl::util::Random::get<unsigned int>(0, 2)));
        break;
    default:
        break;
    }
}

WanderBehavior::WalkData::WalkData()
: dir(Direction::Down)
, steps(0) {}

WanderBehavior::Data::Data()
: walking() {}

} // namespace component
} // namespace core
