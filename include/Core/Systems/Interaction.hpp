#ifndef CORE_SYSTEMS_INTERACTION_HPP
#define CORE_SYSTEMS_INTERACTION_HPP

#include <BLIB/Entities.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief The main interaction system. Manages interaction between entities. Handles item logic and
 *        NPC conversations followed by trainer battles if applicable
 *
 * @ingroup Systems
 *
 */
class Interaction {
public:
    /**
     * @brief Construct a new Interaction system
     *
     * @param owner The primary Systems object
     */
    Interaction(Systems& owner);

    /**
     * @brief Performs an interation on behalf of the given entity
     *
     * @param interactor The entity doing the interaction
     * @return True if an interaction occurred, false if nothing happened
     */
    bool interact(bl::entity::Entity interactor);

private:
    Systems& owner;
};

} // namespace system
} // namespace core

#endif
