#ifndef CORE_SYSTEMS_CONTROLLABLE_HPP
#define CORE_SYSTEMS_CONTROLLABLE_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Controllable.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Simple systems for performing batch locks and unlocks on all controllable entities
 *
 * @ingroup Systems
 *
 */
class Controllable {
public:
    /**
     * @brief Construct a new Controllable systems
     *
     * @param owner The primary systems object
     */
    Controllable(Systems& owner);

    /**
     * @brief Creates the entity view
     *
     */
    void init();

    /**
     * @brief Sets the lock state for all controllable entities
     *
     * @param locked True to lock, false to unlock
     * @param preserve True to remember previous lock state, false to ignore
     */
    void setAllLocks(bool locked, bool preserve = true);

    /**
     * @brief Resets all controllable entity locks to the last stored state
     *
     */
    void resetAllLocks();

private:
    Systems& owner;
    bl::entity::Registry::View<component::Controllable>::Ptr entities;
};

} // namespace system
} // namespace core

#endif
