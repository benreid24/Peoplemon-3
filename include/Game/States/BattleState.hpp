#ifndef GAME_STATES_BATTLESTATE_HPP
#define GAME_STATES_BATTLESTATE_HPP

#include <Core/Battles/Battle.hpp>
#include <Core/Events/BagMenu.hpp>
#include <Core/Events/PeoplemonMenu.hpp>
#include <Core/Systems/Systems.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief This game state does all battling. When the battle is complete it will pop itself from the
 *        engine state stack, returning the game to its previous state.
 *
 * @ingroup States
 *
 */
class BattleState
: public State
, public bl::event::Listener<core::event::OpenPeoplemonMenu, core::event::OpenBagMenu> {
public:
    /**
     * @brief Creates a new BattleState
     *
     * @param systems The main game systems
     * @param battle The battle to run. Must be fully initialized
     * @return bl::engine::State::Ptr The new game state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems,
                                         std::unique_ptr<core::battle::Battle>&& battle);

    /**
     * @brief Destroy the Battle State object
     *
     */
    virtual ~BattleState() = default;

    /**
     * @brief Returns "BattleState"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Does nothing
     *
     */
    virtual void activate(bl::engine::Engine&) override;

    /**
     * @brief Does nothing
     *
     */
    virtual void deactivate(bl::engine::Engine&) override;

    /**
     * @brief Calls into the battle's update method
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine&, float dt, float) override;

private:
    // TODO - BLIB_UPGRADE - update battle rendering
    std::unique_ptr<core::battle::Battle> battle;

    BattleState(core::system::Systems& systems, std::unique_ptr<core::battle::Battle>&& battle);

    virtual void observe(const core::event::OpenPeoplemonMenu& event) override;
    virtual void observe(const core::event::OpenBagMenu& event) override;
};

} // namespace state
} // namespace game

#endif
