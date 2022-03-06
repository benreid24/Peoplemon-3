#include <Game/States/BattleState.hpp>

#include <Core/Events/Battle.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
namespace
{
core::event::BattleCompleted::Type cast(core::battle::Battle::Type bt) {
    switch (bt) {
    case core::battle::Battle::Type::Trainer:
        return core::event::BattleCompleted::Trainer;
    case core::battle::Battle::Type::WildPeoplemon:
        return core::event::BattleCompleted::WildPeoplemon;
    case core::battle::Battle::Type::Online:
    default:
        return core::event::BattleCompleted::Network;
    }
}
} // namespace

bl::engine::State::Ptr BattleState::create(core::system::Systems& systems,
                                           std::unique_ptr<core::battle::Battle>&& battle) {
    return bl::engine::State::Ptr(
        new BattleState(systems, std::forward<std::unique_ptr<core::battle::Battle>>(battle)));
}

BattleState::BattleState(core::system::Systems& systems,
                         std::unique_ptr<core::battle::Battle>&& battle)
: State(systems)
, battle(std::forward<std::unique_ptr<core::battle::Battle>>(battle)) {}

const char* BattleState::name() const { return "BattleState"; }

void BattleState::activate(bl::engine::Engine& engine) {
    oldView             = engine.window().getView();
    sf::View battleView = oldView;
    const sf::Vector2f ws(core::Properties::WindowWidth(), core::Properties::WindowHeight());
    battleView.setSize(ws);
    battleView.setCenter(ws * 0.5f);
    engine.window().setView(battleView);

    // TODO - music here or in intro state?
}

void BattleState::deactivate(bl::engine::Engine& engine) {
    engine.window().setView(oldView);

    engine.eventBus().dispatch<core::event::BattleCompleted>({cast(battle->type)});

    // TODO - stop battle music?
}

void BattleState::update(bl::engine::Engine& engine, float dt) {
    battle->controller->update();
    battle->view.update(dt);
    if (battle->state.currentStage() == core::battle::BattleState::Stage::Completed) {
        engine.popState();
    }
}

void BattleState::render(bl::engine::Engine& engine, float lag) {
    battle->view.render(engine.window(), lag);
}

} // namespace state
} // namespace game
