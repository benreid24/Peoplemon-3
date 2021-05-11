#ifndef CORE_COMPONENTS_SPINBEHAVIOR_HPP
#define CORE_COMPONENTS_SPINBEHAVIOR_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Controllable.hpp>
#include <Core/Files/Behavior.hpp>

namespace core
{
namespace component
{
/**
 * @brief Add this to an entity to make it spin in place
 *
 * @ingroup Components
 *
 */
class SpinBehavior {
public:
    /// Required for BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 8;

    /**
     * @brief Construct a new Spin Behavior component
     *
     * @param position A handle to the position component
     * @param controllable A handle to the controllable component
     * @param dir The direction to spin in
     */
    SpinBehavior(const bl::entity::Registry::ComponentHandle<Position>& position,
                 const bl::entity::Registry::ComponentHandle<Controllable>& controllable,
                 file::Behavior::Spinning::Direction dir);

    /**
     * @brief Updates the behavior
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

private:
const file::Behavior::Spinning::Direction dir;
    bl::entity::Registry::ComponentHandle<Position> position;
    bl::entity::Registry::ComponentHandle<Controllable> controllable;
    float standTime;
};

} // namespace component
} // namespace core

#endif
