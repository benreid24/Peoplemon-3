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
: State(systems, bl::engine::StateMask::Running)
, state(MapFadein)
, fadeout(nullptr)
, spawnId(0) {}

MainGame::~MainGame() { bl::event::Dispatcher::unsubscribe(this); }

const char* MainGame::name() const { return "MainGame"; }

void MainGame::activate(bl::engine::Engine&) {
    bl::event::Dispatcher::subscribe(this);
    if (state == MapFadein) {
        systems.controllable().setAllLocks(true);
        fadeout = systems.engine()
                      .renderer()
                      .getObserver()
                      .getRenderGraph()
                      .putTask<bl::rc::rgi::FadeEffectTask>(
                          core::Properties::ScreenFadePeriod(), 0.f, 1.f);
    }
}

void MainGame::deactivate(bl::engine::Engine& engine) {
    state = Running;
    engine.renderer().getObserver().getRenderGraph().removeTasks<bl::rc::rgi::FadeEffectTask>();
}

void MainGame::update(bl::engine::Engine&, float dt, float) {
    switch (state) {
    case SwitchMapFadeout:
        if (fadeout->complete()) {
            BL_LOG_INFO << "Switching to map: " << replacementMap << " spawn " << spawnId;
            if (!systems.world().switchMaps(replacementMap, spawnId)) {
                BL_LOG_ERROR << "Failed to switch maps";
                systems.engine().flags().set(bl::engine::Flags::Terminate);
                return;
            }

            state   = MapFadein;
            fadeout = systems.engine()
                          .renderer()
                          .getObserver()
                          .getRenderGraph()
                          .putTask<bl::rc::rgi::FadeEffectTask>(
                              core::Properties::ScreenFadePeriod(), 0.f, 1.f);
        }
        break;

    case MapFadein:
        if (fadeout->complete()) {
            state   = Running;
            systems.controllable().resetAllLocks();
            fadeout = nullptr;
            systems.engine()
                .renderer()
                .getObserver()
                .getRenderGraph()
                .removeTask<bl::rc::rgi::FadeEffectTask>();
        }
        break;

    default:
        break;
    }
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
    fadeout        = systems.engine()
                  .renderer()
                  .getObserver()
                  .getRenderGraph()
                  .putTask<bl::rc::rgi::FadeEffectTask>(core::Properties::ScreenFadePeriod());
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
