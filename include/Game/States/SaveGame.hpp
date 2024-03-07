#ifndef GAME_STATES_SAVEGAME_HPP
#define GAME_STATES_SAVEGAME_HPP

#include <BLIB/Graphics/Sprite.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Basic engine state for when the game is saved
 *
 * @ingroup States
 */
class SaveGame : public State {
public:
    /**
     * @brief Creates the save game state
     *
     * @param systems The primary systems object
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Save Game state
     */
    virtual ~SaveGame() = default;

    /**
     * @brief Returns "SaveGame"
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
    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    bl::rc::res::TextureRef bgndTxtr;
    bl::gfx::Sprite background;

    SaveGame(core::system::Systems& systems);
    void onFinish();
};

} // namespace state
} // namespace game

#endif
