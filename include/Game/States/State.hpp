#ifndef GAME_STATES_STATE_HPP
#define GAME_STATES_STATE_HPP

#include <BLIB/Engine/State.hpp>
#include <Core/Systems/Systems.hpp>

/**
 * @defgroup Game
 * @brief All of the functionality of the game itself
 */

/**
 * @addtogroup States
 * @brief Collection of each possible game state
 * @ingroup Game
 */

/// Parent namespace for all functionality unique to the game
namespace game
{
/// Parent namespace for all engine states implemented by the game
namespace state
{
/**
 * @brief Parent to all game states. Provides some commonly required data like core game systems
 *
 * @ingroup States
 */
class State : public bl::engine::State {
public:
    /**
     * @brief Destroy the State object
     */
    virtual ~State() = default;

    /**
     * @brief Return the name of the state
     */
    virtual const char* name() const override = 0;

    /**
     * @brief Make the state active. This is called before update() or render() and called exactly
     *        once until deactivate() is called
     *
     * @param engine The active engine
     */
    virtual void activate(bl::engine::Engine& engine) override = 0;

    /**
     * @brief The state is now not the current state. Called after activate()
     *
     * @param engine The active engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override = 0;

    /**
     * @brief Perform all update logic as required in here
     *
     * @param engine The active engine
     * @param dt The elapsed simulated time since the last call to update
     * @param realDt The elapsed real time since the last call to update
     */
    virtual void update(bl::engine::Engine& engine, float dt, float realDt) override = 0;

protected:
    core::system::Systems& systems;

    /**
     * @brief Initialize the state
     *
     * @param systems A reference to the core game systems
     * @param mask The state mask
     */
    State(core::system::Systems& systems, bl::engine::StateMask::V mask);
};

} // namespace state
} // namespace game

#endif
