#include <Game/States/BattleState.hpp>

#include <Core/Debug/DebugBanner.hpp>
#include <Core/Events/Battle.hpp>
#include <Core/Properties.hpp>
#include <Game/States/BagMenu.hpp>
#include <Game/States/PeoplemonMenu.hpp>

namespace game
{
namespace state
{
namespace
{
class DummyController : public core::battle::BattleController {
public:
    DummyController()          = default;
    virtual ~DummyController() = default;

private:
    sf::Clock timer;

    virtual void onCommandQueued(const core::battle::Command&) override {}
    virtual void onCommandProcessed(const core::battle::Command&) override {}
    virtual void onUpdate(bool, bool) override {
        if (timer.getElapsedTime().asSeconds() >= 3.f) {
            state->setStage(core::battle::BattleState::Stage::Completed);
        }
    }
};

} // namespace

bl::engine::State::Ptr BattleState::create(core::system::Systems& systems,
                                           std::unique_ptr<core::battle::Battle>&& battle) {
    return bl::engine::State::Ptr(
        new BattleState(systems, std::forward<std::unique_ptr<core::battle::Battle>>(battle)));
}

BattleState::BattleState(core::system::Systems& systems,
                         std::unique_ptr<core::battle::Battle>&& battle)
: State(systems, bl::engine::StateMask::Menu)
, battle(std::forward<std::unique_ptr<core::battle::Battle>>(battle)) {
    if (!this->battle->controller) {
        BL_LOG_WARN << "Invalid battle controller, using dummy";
        this->battle->setController(std::make_unique<DummyController>());
    }
}

const char* BattleState::name() const { return "BattleState"; }

void BattleState::activate(bl::engine::Engine& engine) {
    /*engine.renderSystem().cameras().pushCamera(bl::render::camera::StaticCamera::create(
        {sf::Vector2f{0.f, 0.f}, core::Properties::WindowSize()}));
    engine.renderSystem().cameras().configureView(engine.window());
    battle->view.configureView(engine.window().getView());*/
    systems.engine().inputSystem().getActor().addListener(battle->view);
    bl::event::Dispatcher::subscribe(this);
}

void BattleState::deactivate(bl::engine::Engine& engine) {
    // engine.renderSystem().cameras().popCamera();
    systems.engine().inputSystem().getActor().removeListener(battle->view);
    bl::event::Dispatcher::unsubscribe(this);
    bl::audio::AudioSystem::popPlaylist();
}

void BattleState::update(bl::engine::Engine& engine, float dt, float) {
    battle->controller->update();
    battle->view.update(dt);
    battle->state.localPlayer().refresh();
    battle->state.enemy().refresh();
    if (battle->state.currentStage() == core::battle::BattleState::Stage::Completed) {
        engine.popState();
        if (battle->type == core::battle::Battle::Type::Online) {
            systems.player().state().healPeoplemon();
        }
        else {
            if (!battle->result.localPlayerWon) { systems.player().whiteout(); }
        }

        bl::event::Dispatcher::dispatch<core::event::BattleCompleted>(
            {battle->type, std::move(battle->result)});
    }
}

void BattleState::observe(const core::event::OpenPeoplemonMenu& event) {
    systems.engine().pushState(
        PeoplemonMenu::create(systems, event.context, event.outNow, event.chosen));
}

void BattleState::observe(const core::event::OpenBagMenu& event) {
    systems.engine().pushState(
        BagMenu::create(systems, event.context, event.result, event.outNow, event.chosenPeoplemon));
}

} // namespace state
} // namespace game
