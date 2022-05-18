#include <Game/States/BattleState.hpp>

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
sf::Clock timer;
class DummyController : public core::battle::BattleController {
public:
    DummyController()
    : time(0.f) {
        timer.restart();
    }

    virtual ~DummyController() = default;

private:
    float time;

    virtual void onCommandQueued(const core::battle::Command&) override {}
    virtual void onCommandProcessed(const core::battle::Command&) override {}
    virtual void onUpdate(bool, bool) override {
        time += timer.getElapsedTime().asSeconds();
        timer.restart();
        if (time >= 3.f) { state->setStage(core::battle::BattleState::Stage::Completed); }
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
: State(systems)
, battle(std::forward<std::unique_ptr<core::battle::Battle>>(battle)) {
    if (!this->battle->controller) {
        BL_LOG_WARN << "Invalid battle controller, using dummy";
        this->battle->setController(std::move(std::make_unique<DummyController>()));
    }
}

const char* BattleState::name() const { return "BattleState"; }

void BattleState::activate(bl::engine::Engine& engine) {
    oldView             = engine.window().getView();
    sf::View battleView = oldView;
    const sf::Vector2f ws(core::Properties::WindowWidth(), core::Properties::WindowHeight());
    battleView.setSize(ws);
    battleView.setCenter(ws * 0.5f);
    engine.window().setView(battleView);
    battle->view.configureView(battleView);
    systems.player().inputSystem().addListener(battle->view);
    engine.eventBus().subscribe(this);

    // TODO - music here or in intro state?
}

void BattleState::deactivate(bl::engine::Engine& engine) {
    engine.window().setView(oldView);
    systems.player().inputSystem().removeListener(battle->view);

    engine.eventBus().unsubscribe(this);
    engine.eventBus().dispatch<core::event::BattleCompleted>(
        {battle->type, battle->localPlayerWon});

    // TODO - stop battle music?
    // TODO - handle whiteout and evolve
}

void BattleState::update(bl::engine::Engine& engine, float dt) {
    systems.player().inputSystem().update();
    battle->controller->update();
    battle->view.update(dt);
    battle->state.localPlayer().refresh();
    battle->state.enemy().refresh();
    if (battle->state.currentStage() == core::battle::BattleState::Stage::Completed) {
        engine.popState();
        // TODO - handle whiteout/healing here
    }
}

void BattleState::render(bl::engine::Engine& engine, float lag) {
    battle->view.render(engine.window(), lag);
    engine.window().display();
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
