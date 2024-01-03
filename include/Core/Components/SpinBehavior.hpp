#ifndef CORE_COMPONENTS_SPINBEHAVIOR_HPP
#define CORE_COMPONENTS_SPINBEHAVIOR_HPP

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
    /**
     * @brief Construct a new Spin Behavior component
     *
     * @param dir The direction to spin in
     */
    SpinBehavior(file::Behavior::Spinning::Direction dir);

    /**
     * @brief Updates the behavior
     *
     * @param position The position component of the entity
     * @param controller The controllable component of the entity
     * @param dt Time elapsed in seconds
     */
    void update(bl::tmap::Position& position, Controllable& controller, float dt);

private:
    const file::Behavior::Spinning::Direction dir;
    float standTime;
};

} // namespace component
} // namespace core

#endif
