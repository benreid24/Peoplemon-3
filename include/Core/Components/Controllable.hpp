#ifndef CORE_COMPONENTS_CONTROLLABLE_HPP
#define CORE_COMPONENTS_CONTROLLABLE_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Command.hpp>
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
     * @brief Processes the given command and manipulates the entity accordingly
     *
     * @param command The command to apply
     * @return True if the control had effect, false if no effect
     */
    bool processControl(Command command);

private:
    bl::entity::Entity owner;
    system::Systems& systems;
};

} // namespace component
} // namespace core

#endif
