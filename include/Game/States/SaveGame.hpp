#ifndef GAME_STATES_SAVEGAME_HPP
#define GAME_STATES_SAVEGAME_HPP

#include <Game/States/State.hpp>

namespace game
{
namespace state
{
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
     *
     */
    virtual ~SaveGame() = default;

    /**
     * @brief Returns "SaveGame"
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
    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    // TODO - BLIB_UPGRADE - update save game rendering

    bl::resource::Ref<sf::Texture> bgndTxtr;
    sf::Sprite background;

    SaveGame(core::system::Systems& systems);
    void onFinish();
};

} // namespace state
} // namespace game

#endif
