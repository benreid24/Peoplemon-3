#ifndef CORE_SYSTEMS_WILDPEOPLEMON_HPP
#define CORE_SYSTEMS_WILDPEOPLEMON_HPP

#include <BLIB/Events.hpp>
#include <Core/Events/EntityMoved.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Manages the appearances of wild peoplemon when walking through catch tiles
 *
 * @ingroup Systems
 *
 */
class WildPeoplemon : bl::event::Listener<event::EntityMoveFinished> {
public:
    /**
     * @brief Construct a new Wild Peoplemon system
     *
     * @param owner The primary game systems
     */
    WildPeoplemon(Systems& owner);

    /**
     * @brief Subscribes to the game event bus
     *
     */
    void init();

private:
    Systems& owner;
    pplmn::OwnedPeoplemon currentWild;

    virtual void observe(const event::EntityMoveFinished& event) override;
};

} // namespace system
} // namespace core

#endif
