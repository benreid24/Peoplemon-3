#ifndef GAME_STATES_MAINMENU_HPP
#define GAME_STATES_MAINMENU_HPP

#include <BLIB/Engine/State.hpp>

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
    virtual ~MainMenu() = default;

    virtual const char* name() const override;

    virtual void makeActive(bl::engine::Engine&) override;

    virtual void onPushedDown(bl::engine::Engine&) override;

    virtual void onPoppedOff(bl::engine::Engine&) override;

    virtual void update(bl::engine::Engine&, float dt) override;

    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    // menu components
};

} // namespace state
} // namespace game

#endif
