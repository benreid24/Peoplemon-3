#ifndef GAME_STATES_LOADGAME_HPP
#define GAME_STATES_LOADGAME_HPP

#include <Core/Maps/Map.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Provides a menu to select a save and loads the selected save
 * 
 * @ingroup States
 *
 */
class LoadGame : public State {
public:
    /**
     * @brief Creates the new game state
     *
     * @param systems The primary systems object
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the New Game state
     *
     */
    virtual ~LoadGame() = default;

    /**
     * @brief Returns "LoadGame"
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
     * @brief Deactivates the state
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the state and menus and whatnot
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the new game features
     *
     * @param engine The game engine
     * @param lag Time elapsed not accounted for in update
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    sf::Text tempText;
    float aliveTime;

    LoadGame(core::system::Systems& systems);
};
} // namespace state
} // namespace game

#endif