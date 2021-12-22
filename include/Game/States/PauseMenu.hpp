#ifndef GAME_STATES_PAUSEMENU_HPP
#define GAME_STATES_PAUSEMENU_HPP

#include <Game/States/State.hpp>

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Player/Input/MenuDriver.hpp>

namespace game
{
namespace state
{
/**
 * @brief Game state for the pause menu
 *
 * @ingroup States
 *
 */
class PauseMenu : public State {
public:
    /**
     * @brief Creates a new pause menu state
     *
     * @param systems The main game systems
     * @return bl::engine::State::Ptr The created engine state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Pause Menu object
     *
     */
    virtual ~PauseMenu() = default;

    /**
     * @brief Returns "PauseMenu"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Subscribes to event buses
     *
     */
    virtual void activate(bl::engine::Engine&) override;

    /**
     * @brief Unsubscribes from event buses
     *
     */
    virtual void deactivate(bl::engine::Engine&) override;

    /**
     * @brief Updates systems that are not paused
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine&, float dt) override;

    /**
     * @brief Renders the world and the menu overlayed on top of it
     *
     * @param lag Time elapsed not accounted for in update()
     */
    virtual void render(bl::engine::Engine&, float lag) override;

private:
    sf::RectangleShape menuBackground;
    bl::menu::Menu menu;
    core::player::input::MenuDriver inputDriver;

    bl::menu::Item::Ptr resume;
    bl::menu::Item::Ptr ppldex;
    bl::menu::Item::Ptr pplmon;
    bl::menu::Item::Ptr bag;
    bl::menu::Item::Ptr map;
    bl::menu::Item::Ptr save;
    bl::menu::Item::Ptr settings;
    bl::menu::Item::Ptr quit;

    PauseMenu(core::system::Systems& systems);
};

} // namespace state
} // namespace game

#endif
