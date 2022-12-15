#include <Game/States/SaveGame.hpp>

#include <Core/Files/GameSave.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr SaveGame::create(core::system::Systems& s) {
    return bl::engine::State::Ptr(new SaveGame(s));
}

SaveGame::SaveGame(core::system::Systems& s)
: State(s) {
    bgndTxtr = TextureManager::load(
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "savegame.png"));
    background.setTexture(*bgndTxtr, true);
}

const char* SaveGame::name() const { return "SaveGame"; }

void SaveGame::activate(bl::engine::Engine& engine) {
    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));

    const auto cb = std::bind(&SaveGame::onFinish, this);
    if (core::file::GameSave::saveGame(systems.player().state().name)) {
        systems.hud().displayMessage(systems.player().state().name + " saved the game!", cb);
    }
    else { systems.hud().displayMessage("Failed to save the game, goodluck", cb); }
}

void SaveGame::deactivate(bl::engine::Engine& engine) {
    engine.renderSystem().cameras().popCamera();
}

void SaveGame::update(bl::engine::Engine&, float dt) { systems.hud().update(dt); }

void SaveGame::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    engine.window().draw(background);
    systems.hud().render(engine.window(), lag);
    engine.window().display();
}

void SaveGame::onFinish() { systems.engine().popState(); }

} // namespace state
} // namespace game
