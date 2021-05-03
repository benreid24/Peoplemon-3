#ifndef CORE_COMPONENTS_CONTROLLABLE_HPP
#define CORE_COMPONENTS_CONTROLLABLE_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Control.hpp>
#include <Core/Components/Movable.hpp>

namespace core
{
namespace system
{
class Systems;
}

namespace component
{
/**
 * @brief Adding this component to an entity allows it to be controlled
 *
 * @ingroup Components
 *
 */
class Controllable {
public:
    /// Required for BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 5;

    /**
     * @brief Construct a new Controllable component
     *
     * @param systems The primary systems object
     * @param owner The entity that owns this component
     */
    Controllable(system::Systems& systems, bl::entity::Entity owner);

    /**
     * @brief Processes the given control and manipulates the entity accordingly
     *
     * @param control The control to apply
     * @return True if the control had effect, false if no effect
     */
    bool processControl(Control control);

private:
    bl::entity::Entity owner;
    system::Systems& systems;
};

} // namespace component
} // namespace core

#endif
