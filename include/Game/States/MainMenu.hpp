#ifndef GAME_STATES_MAINMENU_HPP
#define GAME_STATES_MAINMENU_HPP

#include <BLIB/Engine/State.hpp>
#include <BLIB/Menu.hpp>
#include <BLIB/Resources.hpp>

/**
 * @defgroup States
 * @brief Collection of each possible game state
 *
 */

/// Parent namespace for all functionality unique to the game
namespace game
{
/// Parent namespace for all engine states implemented by the game
namespace state
{
/**
 * @brief Provides the main menu as an engine state
 *
 * @ingroup States
 *
 */
class MainMenu : public bl::engine::State {
public:
    static bl::engine::State::Ptr create();

    virtual ~MainMenu() = default;

    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine& engine) override;

    virtual void deactivate(bl::engine::Engine& engine) override;

    virtual void update(bl::engine::Engine&, float dt) override;

    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    bl::resource::Resource<sf::Texture>::Ref backgroundTxtr;
    sf::Sprite background;

    std::shared_ptr<bl::menu::Menu> menu;
    std::shared_ptr<bl::menu::KeyboardEventGenerator> keyboardEventGenerator;
    std::shared_ptr<bl::menu::MouseEventGenerator> mouseEventGenerator;
    bl::menu::BasicRenderer renderer;
    bl::menu::ArrowSelector::Ptr selector;
    bl::menu::Item::Ptr newGame;
    bl::menu::Item::Ptr loadGame;
    bl::menu::Item::Ptr settings;
    bl::menu::Item::Ptr quit;

    MainMenu();
};

} // namespace state
} // namespace game

#endif
