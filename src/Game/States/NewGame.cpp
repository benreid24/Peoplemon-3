#include <Game/States/NewGame.hpp>

#include <Core/Properties.hpp>
#include <Game/States/MainGame.hpp>

namespace game
{
namespace state
{
NewGame::Ptr NewGame::create(core::system::Systems& systems) { return Ptr(new NewGame(systems)); }

NewGame::NewGame(core::system::Systems& systems)
: State(systems)
, aliveTime(0.f) {
    tempText.setFont(core::Properties::MenuFont());
    tempText.setCharacterSize(32);
    tempText.setFillColor(sf::Color::Red);
    tempText.setPosition(250, 270);
    tempText.setString("New game stuff here");
}

const char* NewGame::name() const { return "NewGame"; }

void NewGame::activate(bl::engine::Engine&) {
    // do nothing for now
}

void NewGame::deactivate(bl::engine::Engine&) {
    if (!systems.world().switchMaps("WorldMap.map", 5)) {
        BL_LOG_ERROR << "Failed to load world map";
        systems.engine().flags().set(bl::engine::Flags::Terminate);
    }
}

void NewGame::update(bl::engine::Engine&, float dt) {
    aliveTime += dt;
    if (aliveTime >= 2.f) { systems.engine().replaceState(MainGame::create(systems)); }
}

void NewGame::render(bl::engine::Engine&, float) {
    systems.engine().window().clear();
    systems.engine().window().draw(tempText);
    systems.engine().window().display();
}

} // namespace state
} // namespace game
