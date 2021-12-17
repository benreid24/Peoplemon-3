#include <Game/States/LoadGame.hpp>

#include <Core/Files/GameSave.hpp>
#include <Core/Properties.hpp>
#include <Game/States/MainGame.hpp>

namespace game
{
namespace state
{
LoadGame::Ptr LoadGame::create(core::system::Systems& s) { return Ptr(new LoadGame(s)); }

LoadGame::LoadGame(core::system::Systems& s)
: State(s)
, aliveTime(0.f) {
    tempText.setFont(core::Properties::MenuFont());
    tempText.setCharacterSize(32);
    tempText.setFillColor(sf::Color::Blue);
    tempText.setPosition(250, 270);
    tempText.setString("Load game stuff here");
}

const char* LoadGame::name() const { return "LoadGame"; }

void LoadGame::activate(bl::engine::Engine&) {
    // do nothing for now
}

void LoadGame::deactivate(bl::engine::Engine&) {
    core::file::GameSave save;
    save.saveName = "testing";

    if (!save.load(systems.engine().eventBus())) {
        BL_LOG_ERROR << "Failed to load game save";
        systems.engine().flags().set(bl::engine::Flags::Terminate);
    }
}

void LoadGame::update(bl::engine::Engine&, float dt) {
    aliveTime += dt;
    if (aliveTime >= 2.f) { systems.engine().replaceState(MainGame::create(systems)); }
}

void LoadGame::render(bl::engine::Engine&, float) {
    systems.engine().window().clear();
    systems.engine().window().draw(tempText);
    systems.engine().window().display();
}

} // namespace state
} // namespace game
