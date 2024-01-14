#ifndef GAME_STATES_FLYMAP_HPP
#define GAME_STATES_FLYMAP_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Maps/Town.hpp>
#include <Game/States/State.hpp>
#include <SFML/Graphics.hpp>

namespace game
{
namespace state
{
/**
 * @brief Engine state for the fly map
 *
 * @ingroup States
 */
class FlyMap
: public State
, public bl::input::Listener {
public:
    /**
     * @brief Creates the fly map state
     *
     * @param systems The primary systems object
     * @param unpause Boolean param to set if the pause menu should close after this closes
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems, bool& unpause);

    /**
     * @brief Destroy the FlyMap state
     */
    virtual ~FlyMap() = default;

    /**
     * @brief Returns "FlyMap"
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
    bool& unpause;

    bl::rc::res::TextureRef mapTxtr;
    bl::rc::res::TextureRef townTxtr;
    bl::rc::res::TextureRef cursorTxtr;
    bl::rc::res::TextureRef playerTxtr;
    bl::gfx::Sprite map;
    bl::gfx::BatchedSprites towns;
    bl::gfx::Sprite player;
    bl::gfx::Sprite cursor;

    bl::rc::res::TextureRef panelTxtr;
    bl::gfx::Sprite panel;
    bl::gfx::Text townName;
    bl::gfx::Text townDesc;

    bl::menu::Menu townMenu;
    core::input::MenuDriver inputDriver;
    unsigned int selected;

    FlyMap(core::system::Systems& systems, bool& unpause);
    void clearHover();
    void hoverTown(unsigned int i);
    void selectTown(unsigned int i);
    void onFlyChoice(const std::string& choice, const core::map::Town& town);
    void messageDone();
    void close();

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace state
} // namespace game

#endif
