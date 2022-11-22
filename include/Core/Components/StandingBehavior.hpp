#ifndef CORE_COMPONENTS_STANDINGBEHAVIOR_HPP
#define CORE_COMPONENTS_STANDINGBEHAVIOR_HPP

#include <Core/Components/Controllable.hpp>

namespace core
{
namespace component
{
/**
 * @brief AI behavior for standing in place facing a fixed direction
 *
 * @ingroup Components
 *
 */
class StandingBehavior {
public:
    /**
     * @brief Construct a new Standing Behavior component
     *
     * @param faceDir The direction to face
     */
    StandingBehavior(Direction faceDir);

    /**
     * @brief Ensures the managed entity is facing the required position
     *
     * @param position The entities position component
     * @param controller The entities controllable component
     *
     */
    void update(Position& position, Controllable& controller);

private:
    const Direction dir;
};

} // namespace component
} // namespace core

#endif
