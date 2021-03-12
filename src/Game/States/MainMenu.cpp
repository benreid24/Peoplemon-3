#include <BLIB/Engine/Engine.hpp>
#include <Game/States/MainMenu.hpp>

using bl::engine::Engine;

namespace game
{
namespace state
{
const char* MainMenu::name() const { return "MainMenu"; }

void MainMenu::makeActive(Engine&) {
    // reset menu state
}

void MainMenu::onPoppedOff(Engine&) {
    // do nothing?
}

void MainMenu::onPushedDown(Engine&) {
    // do nothing
}

void MainMenu::update(Engine&, float dt) {
    // update menu
}

void MainMenu::render(Engine& engine, float lag) {
    // render menu
}

} // namespace state
} // namespace game
