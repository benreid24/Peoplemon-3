#include <Game/States/MainGame.hpp>

#include <Game/States/MapExplorer.hpp>
#include <Game/States/PauseMenu.hpp>

namespace game
{
namespace state
{
MainGame::Ptr MainGame::create(core::system::Systems& systems) {
    return Ptr(new MainGame(systems));
}

MainGame::MainGame(core::system::Systems& systems)
: State(systems) {}

const char* MainGame::name() const { return "MainGame"; }

void MainGame::activate(bl::engine::Engine&) { systems.engine().eventBus().subscribe(this); }

void MainGame::deactivate(bl::engine::Engine&) { systems.engine().eventBus().unsubscribe(this); }

void MainGame::update(bl::engine::Engine&, float dt) {
    systems.update(dt);

    // TODO - fade in/outs on map transitions
}

void MainGame::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    systems.render().render(engine.window(), lag);
    engine.window().display();
}

void MainGame::observe(const core::event::StateChange& event) {
    switch (event.type) {
    case core::event::StateChange::GamePaused:
        systems.engine().pushState(PauseMenu::create(systems));
        break;

    default:
        break;
    }
}

void MainGame::observe(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
#ifdef PEOPLEMON_DEBUG
        if (event.key.code == sf::Keyboard::F1) {
            systems.engine().pushState(MapExplorer::create(systems));
        }
#endif
    }
}

} // namespace state
} // namespace game
