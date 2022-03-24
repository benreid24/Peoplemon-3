#ifndef GAME_STATES_BATTLESTATE_HPP
#define GAME_STATES_BATTLESTATE_HPP

#include <Core/Battles/Battle.hpp>
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
class BattleState : public State {
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
    virtual void update(bl::engine::Engine&, float dt) override;

    /**
     * @brief Renders the battle to the display
     *
     * @param lag Time elapsed in seconds since previous update()
     */
    virtual void render(bl::engine::Engine&, float lag) override;

private:
    std::unique_ptr<core::battle::Battle> battle;
    sf::View oldView;

    BattleState(core::system::Systems& systems, std::unique_ptr<core::battle::Battle>&& battle);
};

} // namespace state
} // namespace game

#endif
