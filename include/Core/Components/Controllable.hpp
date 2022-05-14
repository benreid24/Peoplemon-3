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
     * @param overrideLock True to process the control even if locked
     * @return True if the control had effect, false if no effect
     */
    bool processControl(Command command, bool overrideLock = false);

    /**
     * @brief Locks the controllable and prevents any commands from being processed. Optionally
     *        remembers the previous lock state to reset back to
     *
     * @param lock True to lock, false to unlock
     * @param preserve True to remember previous lock state, false to discard
     */
    void setLocked(bool lock, bool preserve = true);

    /**
     * @brief Resets the lock state back to the last preserved state
     *
     */
    void resetLock();

    /**
     * @brief Returns whether or not this component is locked
     *
     */
    bool isLocked() const;

private:
    bl::entity::Entity owner;
    system::Systems& systems;
    bool locked;
    bool wasLocked;
};

} // namespace component
} // namespace core

#endif
