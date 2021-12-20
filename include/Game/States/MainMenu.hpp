#ifndef GAME_STATES_MAINMENU_HPP
#define GAME_STATES_MAINMENU_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Player/Input/MenuDriver.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Provides the main menu as an engine state
 *
 * @ingroup States
 *
 */
class MainMenu : public State {
public:
    /**
     * @brief Creates a new MainMenu state
     *
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /// Cleans up all resources
    virtual ~MainMenu() = default;

    /**
     * @brief Returns "MainMenu";
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Subscribes the menu to the engine event bus
     *
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Unsubscribes the menu to the engine event bus
     *
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Does nothing
     *
     */
    virtual void update(bl::engine::Engine&, float dt) override;

    /**
     * @brief Renders the menu and its background
     *
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    bl::resource::Resource<sf::Texture>::Ref backgroundTxtr;
    sf::Sprite background;

    bl::menu::Menu menu;
    core::player::input::MenuDriver inputDriver;
    bl::menu::TextItem::Ptr newGame;
    bl::menu::TextItem::Ptr loadGame;
    bl::menu::TextItem::Ptr settings;
    bl::menu::TextItem::Ptr quit;

    MainMenu(core::system::Systems& systems);
};

} // namespace state
} // namespace game

#endif
