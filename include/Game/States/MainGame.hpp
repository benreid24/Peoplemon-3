#ifndef GAME_STATES_MAINGAME_HPP
#define GAME_STATES_MAINGAME_HPP

#include <Core/Events/Battle.hpp>
#include <Core/Events/Maps.hpp>
#include <Core/Events/StateChange.hpp>
#include <Core/Events/StorageSystem.hpp>
#include <Core/Events/Store.hpp>
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
, public bl::event::Listener<sf::Event, core::event::StateChange, core::event::SwitchMapTriggered,
                             core::event::BattleStarted, core::event::StoreOpened,
                             core::event::StorageSystemOpened> {
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
    virtual ~MainGame();

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
    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    enum GameState { SwitchMapFadeout, MapFadein, Running };

    GameState state;
    bl::rc::rgi::FadeEffectTask* fadeout;

    std::string replacementMap;
    int spawnId;

    MainGame(core::system::Systems& systems);

    virtual void observe(const sf::Event& event) override;
    virtual void observe(const core::event::StateChange& event) override;
    virtual void observe(const core::event::SwitchMapTriggered& event) override;
    virtual void observe(const core::event::BattleStarted& event) override;
    virtual void observe(const core::event::StoreOpened& event) override;
    virtual void observe(const core::event::StorageSystemOpened& event) override;
};

} // namespace state
} // namespace game

#endif
