#ifndef GAME_STATES_PAUSEMENU_HPP
#define GAME_STATES_PAUSEMENU_HPP

#include <Game/States/State.hpp>

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Menu.hpp>
#include <Core/Player/Input/MenuDriver.hpp>

namespace game
{
namespace state
{
class PauseMenu : public State {
public:
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    virtual ~PauseMenu() = default;

    virtual const char* name() const override;

    virtual void activate(bl::engine::Engine&) override;

    virtual void deactivate(bl::engine::Engine&) override;

    virtual void update(bl::engine::Engine&, float dt) override;

    virtual void render(bl::engine::Engine&, float lag) override;

private:
    sf::RectangleShape menuBackground;
    bl::container::ObjectWrapper<bl::menu::Menu> menu;
    bl::menu::BasicRenderer renderer;
    bl::menu::ArrowSelector::Ptr selector;
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
