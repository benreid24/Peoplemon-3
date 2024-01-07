#ifndef CORE_COMPONENTS_WANDERBEHAVIOR_HPP
#define CORE_COMPONENTS_WANDERBEHAVIOR_HPP

#include <Core/Components/Controllable.hpp>
#include <Core/Components/SpinBehavior.hpp>
#include <Core/Components/StandingBehavior.hpp>

namespace core
{
namespace component
{
/**
 * @brief Add this component to an entity to make it wander around
 *
 * @ingroup Components
 *
 */
class WanderBehavior {
public:
    /**
     * @brief Construct a new Wander Behavior component
     *
     * @param radius The radius to stay within, in tiles
     * @param origin The starting position, in tiles
     */
    WanderBehavior(unsigned int radius, const sf::Vector2i& origin);

    /**
     * @brief Updates the entity and wanders around
     *
     * @param position The position of the entity
     * @param controller The controllable of the entity
     * @param dt Time elapsed, in seconds
     */
    void update(bl::tmap::Position& position, Controllable& controller, float dt);

private:
    struct WalkData {
        bl::tmap::Direction dir;
        unsigned short int total;
        unsigned short int steps;

        WalkData();
    };

    union Data {
        WalkData walking;
        StandingBehavior standing;
        SpinBehavior spinning;

        Data();
    } data;

    enum State : std::uint8_t {
        Walking    = 0,
        Spinning   = 1,
        Standing   = 2,
        NUM_STATES = 2,
        Paused     = 3 // intentionally outside of possible states
    };

    const sf::Vector2i origin;
    const unsigned int radius;
    State state;
    float stateTime;
    float transitionTime;

    void switchState();
};

} // namespace component
} // namespace core

#endif
