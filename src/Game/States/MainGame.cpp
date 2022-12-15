#include <Game/States/MainGame.hpp>

#include <Core/Debug/DebugBanner.hpp>
#include <Core/Properties.hpp>
#include <Core/Scripts/DebugScriptContext.hpp>
#include <Game/States/BattleWrapperState.hpp>
#include <Game/States/Evolution.hpp>
#include <Game/States/MapExplorer.hpp>
#include <Game/States/PauseMenu.hpp>
#include <Game/States/StorageSystem.hpp>
#include <Game/States/StoreMenu.hpp>
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

MainGame::~MainGame() { bl::event::Dispatcher::unsubscribe(this); }

const char* MainGame::name() const { return "MainGame"; }

void MainGame::activate(bl::engine::Engine&) {
    bl::event::Dispatcher::subscribe(this);
    systems.world().activeMap().setupCamera(systems);
}

void MainGame::deactivate(bl::engine::Engine&) {
    systems.engine().renderSystem().cameras().clearViewportConstraint();
}

void MainGame::update(bl::engine::Engine&, float dt) {
    systems.update(dt, true);

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
            // systems.world().activeMap().setupCamera(systems);
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
        }
        else {
            const float a = (1.f - fadeTime / core::Properties::ScreenFadePeriod()) * 255.f;
            cover.setFillColor(sf::Color(0, 0, 0, a));
        }
        break;

    default:
        break;
    }
}

void MainGame::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    systems.render().render(engine.window(), systems.world().activeMap(), lag);

    switch (state) {
    case MapFadein:
    case SwitchMapFadeout:
        cover.setSize(engine.window().getView().getSize());
        cover.setPosition(engine.window().getView().getCenter());
        cover.setOrigin(engine.window().getView().getSize() * 0.5f);
        engine.window().draw(cover);
        break;
    default:
        break;
    }

#ifdef PEOPLEMON_DEBUG
    core::debug::DebugBanner::render(engine.window());
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
        else if (event.key.code == sf::Keyboard::F5) {
            auto& team = systems.player().state().peoplemon;
            if (!team.empty()) {
                auto& ppl = team.front();
                if (ppl.evolvesInto() != core::pplmn::Id::Unknown) {
                    const unsigned int lvl = ppl.evolveLevel();
                    if (lvl <= 100) {
                        while (ppl.currentLevel() < lvl) {
                            const auto move = ppl.levelUp();
                            if (move != core::pplmn::MoveId::Unknown) { ppl.gainMove(move); }
                        }
                        systems.engine().pushState(Evolution::create(systems, ppl));
                    }
                }
            }
        }
    }
#else
    (void)event;
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
    systems.engine().pushState(BattleWrapperState::create(systems, std::move(battle)));
}

void MainGame::observe(const core::event::StoreOpened& store) {
    systems.engine().pushState(StoreMenu::create(systems, store));
}

void MainGame::observe(const core::event::StorageSystemOpened&) {
    systems.engine().pushState(StorageSystem::create(systems));
}

} // namespace state
} // namespace game
