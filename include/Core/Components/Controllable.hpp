#ifndef CORE_COMPONENTS_CONTROLLABLE_HPP
#define CORE_COMPONENTS_CONTROLLABLE_HPP

#include <Core/Components/Movable.hpp>
#include <Core/Input/Control.hpp>

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
    /**
     * @brief Construct a new Controllable component
     *
     * @param systems The primary systems object
     * @param owner The entity that owns this component
     */
    Controllable(system::Systems& systems, bl::ecs::Entity owner);

    /**
     * @brief Processes the given command and manipulates the entity accordingly
     *
     * @param command The command to apply
     * @param sprint True to sprint, false to walk. Only has effect if processing a move control
     * @param overrideLock True to process the control even if locked
     * @return True if the control had effect, false if no effect
     */
    bool processControl(input::EntityControl command, bool sprint = false,
                        bool overrideLock = false);

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
    bl::ecs::Entity owner;
    system::Systems& systems;
    bool locked;
    bool wasLocked;
};

} // namespace component
} // namespace core

#endif
