#ifndef GAME_STATES_LOADGAME_HPP
#define GAME_STATES_LOADGAME_HPP

#include <BLIB/Render.hpp>
#include <Core/Files/GameSave.hpp>
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
class LoadGame
: public State
, public bl::input::Listener {
public:
    /**
     * @brief Creates the load game state
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
    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    enum LoadState { SelectingSave, ChooseAction, SaveDeleted, Fading, Error } state;

    std::vector<core::file::GameSave> saves;
    unsigned int selectedSave;
    bl::rc::res::TextureRef bgndTxtr;
    bl::gfx::Sprite background;
    bl::rc::rgi::FadeEffectTask* fadeout;

    bl::menu::Menu saveMenu;
    bl::menu::Menu actionMenu;
    core::input::MenuDriver inputDriver;

    LoadGame(core::system::Systems& systems);
    void saveSelected(unsigned int save);
    void errorDone();
    void reset();

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace state
} // namespace game

#endif
