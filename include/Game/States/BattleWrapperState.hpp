#ifndef GAME_STATES_BATTLEWRAPPERSTATE_HPP
#define GAME_STATES_BATTLEWRAPPERSTATE_HPP

#include <Game/States/State.hpp>

namespace game
{
namespace state
{
namespace intros
{
class SequenceBase;
}

/**
 * @brief Wrapper state that manages battle intro transitions as well as post-battle evolutions
 *
 * @ingroup States
 *
 */
class BattleWrapperState
: public State
, public bl::input::Listener {
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
     * @brief Destroy the Battle Wrapper State state
     *
     */
    virtual ~BattleWrapperState() = default;

    /**
     * @brief Returns "BattleWrapperState"
     *
     */
    const char* name() const override;

    /**
     * @brief Initiates the battle transition or evolution if applicable, otherwise pops this state
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Does nothing
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the intro sequence
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the intro sequence
     *
     * @param engine The game engine
     * @param lag Time, in seconds, not accounted for in update()
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    enum struct Substate { BattleIntro, Battling, Evolving };

    Substate state;
    std::unique_ptr<core::battle::Battle> battle;
    std::unique_ptr<intros::SequenceBase> sequence;
    sf::View sequenceView;
    unsigned int evolveIndex;

    BattleWrapperState(core::system::Systems& systems,
                       std::unique_ptr<core::battle::Battle>&& battle,
                       std::unique_ptr<intros::SequenceBase>&& sequence);

    void incEvolveIndex();
    void startEvolve();
    virtual bool observe(const bl::input::Actor&, unsigned int, bl::input::DispatchType, bool) override;
};

} // namespace state
} // namespace game

#endif
