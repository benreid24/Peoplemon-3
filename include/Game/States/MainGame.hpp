#ifndef GAME_STATES_MAINGAME_HPP
#define GAME_STATES_MAINGAME_HPP

#include <Core/Events/StateChange.hpp>
#include <Core/Maps/Map.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief The big honcho. This is the primary state that runs a majority of the time
 *
 * @ingroup States
 *
 */
class MainGame
: public State
, public bl::event::Listener<sf::Event, core::event::StateChange> {
public:
    /**
     * @brief Creates the main game state. The game state must be initialized before invoking the
     *        main game state
     *
     * @param systems The primary systems object
     * @return bl::engine::State::Ptr The main game state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Main Game state
     *
     */
    virtual ~MainGame() = default;

    /**
     * @brief Returns "MainGame"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Activates the state
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Deactivates the game state
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates all of the primary systems
     *
     * @param engine The game engine
     * @param dt Time to step forward in the game world
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the map and all entities
     *
     * @param engine The game engine
     * @param lag Time elapsed in not accounted for in update
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    MainGame(core::system::Systems& systems);

    virtual void observe(const sf::Event& event) override;

    virtual void observe(const core::event::StateChange& event) override;
};

} // namespace state
} // namespace game

#endif
