#include <Core/Components/WanderBehavior.hpp>

#include <BLIB/Util/Random.hpp>

namespace core
{
namespace component
{
WanderBehavior::WanderBehavior(unsigned int radius, const sf::Vector2i& origin)
: origin(origin)
, radius(radius)
, state(Walking)
, stateTime(0.f)
, transitionTime(0.f) {
    switchState();
}

void WanderBehavior::update(Position& position, Controllable& controller, float dt) {
    stateTime += dt;
    if (stateTime >= transitionTime && state != Paused) { switchState(); }

    switch (state) {
    case Walking: {
        const auto changeDirection = [this, &position, &controller]() {
            if (bl::util::Random::chance(1, 2)) {
                const int dx   = position.positionTiles().x - origin.x;
                const float xf = dx + radius;
                const float p  = 1.f - xf / static_cast<float>(2 * radius);
                if (bl::util::Random::get<float>(0.f, 1.f) < p) {
                    data.walking.dir = Direction::Right;
                }
                else {
                    data.walking.dir = Direction::Left;
                }
            }
            else {
                const int dy   = position.positionTiles().y - origin.y;
                const float yf = dy + radius;
                const float p  = 1.f - yf / static_cast<float>(2 * radius);
                if (bl::util::Random::get<float>(0.f, 1.f) < p) {
                    data.walking.dir = Direction::Down;
                }
                else {
                    data.walking.dir = Direction::Up;
                }
            }

            state = Paused;
            transitionTime -= stateTime;
            stateTime = 0.f;
        };

        if (controller.processControl(moveControlFromDirection(data.walking.dir))) {
            data.walking.steps += 1;
            if (data.walking.steps >= data.walking.total) changeDirection();
        }
        else if (!position.moving()) {
            changeDirection();
        }
    } break;
    case Standing:
        data.standing.update(position, controller, dt);
        break;
    case Spinning:
        data.spinning.update(position, controller, dt);
        break;
    case Paused:
        if (stateTime >= bl::util::Random::get<float>(1.f, 5.f)) {
            data.walking.total = bl::util::Random::get<unsigned short int>(1, 3);
            if (position.direction != data.walking.dir)
                controller.processControl(moveControlFromDirection(data.walking.dir));
            state = Walking;
        }

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

    state     = static_cast<State>(bl::util::Random::get<int>(0, NUM_STATES));
    stateTime = 0.f;

    switch (state) {
    case Walking:
        new (&data.walking) WalkData();
        data.walking.dir = static_cast<Direction>(bl::util::Random::get<unsigned int>(0, 3));
        transitionTime   = bl::util::Random::get<float>(5.f, 13.f);
        break;
    case Standing:
        new (&data.standing)
            StandingBehavior(static_cast<Direction>(bl::util::Random::get<unsigned int>(0, 3)));
        transitionTime = bl::util::Random::get<float>(1.f, 3.f);
        break;
    case Spinning:
        new (&data.spinning) SpinBehavior(static_cast<file::Behavior::Spinning::Direction>(
            bl::util::Random::get<unsigned int>(0, 2)));
        transitionTime = bl::util::Random::get<float>(1.5f, 3.f);
        break;
    default:
        break;
    }
}

WanderBehavior::WalkData::WalkData()
: dir(Direction::Down)
, total(3)
, steps(0) {}

WanderBehavior::Data::Data()
: walking() {}

} // namespace component
} // namespace core
