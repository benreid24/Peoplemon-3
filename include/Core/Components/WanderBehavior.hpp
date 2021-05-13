#ifndef CORE_COMPONENTS_WANDERBEHAVIOR_HPP
#define CORE_COMPONENTS_WANDERBEHAVIOR_HPP

#include <Core/Components/Controllable.hpp>
#include <Core/Components/SpinBehavior.hpp>
#include <Core/Components/StandingBehavior.hpp>

namespace core
{
namespace component
{
class WanderBehavior {
public:
    static constexpr bl::entity::Component::IdType ComponentId = 10;

    WanderBehavior(unsigned int radius);

    void update(Position& position, Controllable& controller, float dt);

private:
    struct WalkData {
        Direction dir;
        unsigned int steps;

        WalkData();
    };

    union Data {
        WalkData walking;
        StandingBehavior standing;
        SpinBehavior spinning;

        Data();
    } data;

    enum State : std::uint8_t { Walking = 0, Spinning = 1, Standing = 2, NUM_STATES = 2 };

    const unsigned int radius;
    State state;
    float stateTime;
    float transitionTime;

    void switchState();
};

} // namespace component
} // namespace core

#endif
