#include <Game/States/NewGame.hpp>

#include <Core/Properties.hpp>
#include <Game/States/MainGame.hpp>

namespace game
{
namespace state
{
NewGame::Ptr NewGame::create(core::system::Systems& systems) { return Ptr(new NewGame(systems)); }

NewGame::NewGame(core::system::Systems& systems)
: State(systems) {
    tempText.setFont(core::Properties::MenuFont());
    tempText.setCharacterSize(32);
    tempText.setFillColor(sf::Color::Red);
    tempText.setPosition(250, 270);
    tempText.setString("New game stuff here");
}

const char* NewGame::name() const { return "NewGame"; }

void NewGame::activate(bl::engine::Engine&) {
    systems.hud().getInputString("Does this work?", 0, 16, [this](const std::string& i) {
        BL_LOG_INFO << i;
        systems.engine().replaceState(MainGame::create(systems));
    });
}

void NewGame::deactivate(bl::engine::Engine&) {
    if (!systems.world().switchMaps("WorldMap.map", 5)) {
        BL_LOG_ERROR << "Failed to load world map";
        systems.engine().flags().set(bl::engine::Flags::Terminate);
    }
}

void NewGame::update(bl::engine::Engine&, float dt) {
    systems.hud().update(dt);
    systems.player().update();
}

void NewGame::render(bl::engine::Engine&, float lag) {
    systems.engine().window().clear();
    systems.engine().window().draw(tempText);
    systems.hud().render(systems.engine().window(), lag);
    systems.engine().window().display();
}

} // namespace state
} // namespace game
