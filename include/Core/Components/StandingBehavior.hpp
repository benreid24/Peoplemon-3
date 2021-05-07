#ifndef CORE_COMPONENTS_STANDINGBEHAVIOR_HPP
#define CORE_COMPONENTS_STANDINGBEHAVIOR_HPP

#include <BLIB/Entities.hpp>
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
    /// Required for BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 7;

    /**
     * @brief Construct a new Standing Behavior component
     *
     * @param faceDir The direction to face
     * @param position The position component
     * @param controllable The controllable component
     */
    StandingBehavior(Direction faceDir,
                     const bl::entity::Registry::ComponentHandle<Position>& position,
                     const bl::entity::Registry::ComponentHandle<Controllable>& controllable);

    /**
     * @brief Ensures the managed entity is facing the required position
     *
     */
    void update();

private:
    const Direction dir;
    bl::entity::Registry::ComponentHandle<Position> position;
    bl::entity::Registry::ComponentHandle<Controllable> controllable;
};

} // namespace component
} // namespace core

#endif
