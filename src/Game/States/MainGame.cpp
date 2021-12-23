#include <Game/States/MainGame.hpp>

#include <Core/Properties.hpp>
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
: State(systems)
, state(MapFadein)
, fadeTime(0.f)
, spawnId(0) {
    cover.setFillColor(sf::Color::Black);
}

const char* MainGame::name() const { return "MainGame"; }

void MainGame::activate(bl::engine::Engine&) {
    systems.engine().eventBus().subscribe(this);
    if (state == MapFadein) systems.controllable().setAllLocks(true, false);
}

void MainGame::deactivate(bl::engine::Engine&) { systems.engine().eventBus().unsubscribe(this); }

void MainGame::update(bl::engine::Engine&, float dt) {
    switch (state) {
    case SwitchMapFadeout:
        fadeTime += dt;
        if (fadeTime >= core::Properties::ScreenFadePeriod()) {
            BL_LOG_INFO << "Switching to map: " << replacementMap << " spawn " << spawnId;
            if (!systems.world().switchMaps(replacementMap, spawnId)) {
                BL_LOG_ERROR << "Failed to switch maps";
                systems.engine().flags().set(bl::engine::Flags::Terminate);
                return;
            }
            fadeTime = 0.f;
            state    = MapFadein;
        }
        else {
            const float a = fadeTime / core::Properties::ScreenFadePeriod() * 255.f;
            cover.setFillColor(sf::Color(0, 0, 0, a));
        }
        break;

    case MapFadein:
        fadeTime += dt;
        if (fadeTime >= core::Properties::ScreenFadePeriod()) {
            state    = Running;
            fadeTime = 0.f;
            systems.controllable().setAllLocks(false, false);
        }
        else {
            const float a = (1.f - fadeTime / core::Properties::ScreenFadePeriod()) * 255.f;
            cover.setFillColor(sf::Color(0, 0, 0, a));
        }
        break;

    default:
        break;
    }

    systems.update(dt);
}

void MainGame::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    systems.render().render(engine.window(), systems.world().activeMap(), lag);

    switch (state) {
    case MapFadein:
    case SwitchMapFadeout:
        cover.setSize(engine.window().getView().getSize());
        engine.window().draw(cover);
        break;
    default:
        break;
    }

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

void MainGame::observe(const core::event::SwitchMapTriggered& event) {
    replacementMap = event.newMap;
    spawnId        = event.spawn;
    state          = SwitchMapFadeout;
    systems.controllable().setAllLocks(true, false);
}

} // namespace state
} // namespace game
