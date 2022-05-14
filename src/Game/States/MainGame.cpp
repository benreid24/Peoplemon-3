#include <Game/States/MainGame.hpp>

#include <Core/Battles/BattleSkipper.hpp>
#include <Core/Properties.hpp>
#include <Core/Scripts/DebugScriptContext.hpp>
#include <Game/States/BattleState.hpp>
#include <Game/States/MapExplorer.hpp>
#include <Game/States/PauseMenu.hpp>
#include <iostream>

namespace game
{
namespace state
{
namespace
{
void runDebugScript(core::system::Systems& systems, std::atomic_bool& rflag) {
    std::cout << "Enter script: ";
    std::string cmd;
    std::getline(std::cin, cmd);

    bl::script::Script script(cmd);
    if (script.valid()) {
        script.resetContext(core::script::DebugScriptContext(systems));
        script.run(&systems.engine().scriptManager());
    }

    rflag = false;
}

#ifdef PEOPLEMON_DEBUG
sf::Clock textTimer;

class BattleText {
public:
    static void update() {
        if (core::battle::BattleSkipper::skipBattles()) { get().text.setString("Battles Off"); }
        else {
            get().text.setString("Battles On");
        }
        get().text.setPosition(sf::Vector2f(static_cast<float>(core::Properties::WindowWidth()) -
                                                get().text.getGlobalBounds().width - 10.f,
                                            10.f));
    }

    static void render(sf::RenderTarget& target) {
        if (textTimer.getElapsedTime().asSeconds() < 2.f) {
            static const sf::View view = makeView();
            const sf::View oldView     = target.getView();
            target.setView(view);
            target.draw(get().text);
            target.setView(oldView);
        }
    }

    static sf::View makeView() {
        const sf::Vector2f size(core::Properties::WindowWidth(), core::Properties::WindowHeight());
        sf::View view(size * 0.5f, size);
        return view;
    }

private:
    sf::Text text;

    BattleText() {
        text.setFont(core::Properties::MenuFont());
        text.setFillColor(sf::Color::Cyan);
        text.setOutlineColor(sf::Color::Black);
        text.setCharacterSize(20);
        text.setOutlineThickness(2.f);
    }

    static BattleText& get() {
        static BattleText t;
        return t;
    }
};
#endif

} // namespace

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

void MainGame::activate(bl::engine::Engine&) { systems.engine().eventBus().subscribe(this); }

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

        systems.update(dt, true);
        break;

    case MapFadein:
        fadeTime += dt;
        if (fadeTime >= core::Properties::ScreenFadePeriod()) {
            state    = Running;
            fadeTime = 0.f;
        }
        else {
            const float a = (1.f - fadeTime / core::Properties::ScreenFadePeriod()) * 255.f;
            cover.setFillColor(sf::Color(0, 0, 0, a));
        }

        systems.update(dt, false);
        break;

    default:
        systems.update(dt, true);
        break;
    }

#ifdef PEOPLEMON_DEBUG
    BattleText::update();
#endif
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

#ifdef PEOPLEMON_DEBUG
    BattleText::render(engine.window());
#endif

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
#ifdef PEOPLEMON_DEBUG
    if (event.type == sf::Event::KeyPressed) {
        static std::atomic_bool running = false;

        if (event.key.code == sf::Keyboard::F1) {
            systems.engine().pushState(MapExplorer::create(systems));
        }
        else if (event.key.code == sf::Keyboard::Tilde) {
            if (!running) {
                running = true;
                std::thread t(&runDebugScript, std::ref(systems), std::ref(running));
                t.detach();
            }
        }
        else if (event.key.code == sf::Keyboard::F2) {
            core::battle::BattleSkipper::toggleSkipBattles();
            textTimer.restart();
        }
    }
#endif
}

void MainGame::observe(const core::event::SwitchMapTriggered& event) {
    replacementMap = event.newMap;
    spawnId        = event.spawn;
    state          = SwitchMapFadeout;
    systems.controllable().setAllLocks(true, false);
}

void MainGame::observe(const core::event::BattleStarted& event) {
    std::unique_ptr<core::battle::Battle>& battle =
        const_cast<std::unique_ptr<core::battle::Battle>&>(event.battle);
    systems.engine().pushState(BattleState::create(systems, std::move(battle)));
    // TODO - battle intro state
}

} // namespace state
} // namespace game
