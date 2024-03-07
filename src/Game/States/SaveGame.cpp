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
: State(s, bl::engine::StateMask::Menu) {
    bgndTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "savegame.png"));
    background.create(s.engine(), bgndTxtr);
    background.getTransform().setDepth(bl::cam::OverlayCamera::MaxDepth); // ensure behind HUD
}

const char* SaveGame::name() const { return "SaveGame"; }

void SaveGame::activate(bl::engine::Engine& engine) {
    background.addToScene(engine.renderer().getObserver().pushScene<bl::rc::Overlay>(),
                          bl::rc::UpdateSpeed::Static);

    const auto cb = std::bind(&SaveGame::onFinish, this);
    if (core::file::GameSave::saveGame(systems.player().state().name)) {
        systems.hud().displayMessage(systems.player().state().name + " saved the game!", cb);
    }
    else { systems.hud().displayMessage("Failed to save the game, good luck :(", cb); }
}

void SaveGame::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();
}

void SaveGame::update(bl::engine::Engine&, float, float) {}

void SaveGame::onFinish() { systems.engine().popState(); }

} // namespace state
} // namespace game
