#include <Game/States/BattleWrapperState.hpp>

#include "Battle/TrainerIntro.hpp"
#include "Battle/WildIntro.hpp"

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/States/BattleState.hpp>
#include <Game/States/Evolution.hpp>
#include <array>

namespace game
{
namespace state
{
bl::engine::State::Ptr BattleWrapperState::create(core::system::Systems& systems,
                                                  std::unique_ptr<core::battle::Battle>&& battle) {
    std::unique_ptr<intros::SequenceBase> seq;
    if (battle->type == core::battle::Battle::Type::WildPeoplemon) {
        seq = std::make_unique<intros::WildSequence>();
    }
    else { seq = std::make_unique<intros::TrainerSequence>(); }
    return Ptr{new BattleWrapperState(
        systems, std::forward<std::unique_ptr<core::battle::Battle>>(battle), std::move(seq))};
}

BattleWrapperState::BattleWrapperState(core::system::Systems& systems,
                                       std::unique_ptr<core::battle::Battle>&& battle,
                                       std::unique_ptr<intros::SequenceBase>&& sequence)
: State(systems, bl::engine::StateMask::Menu)
, state(Substate::BattleIntro)
, battle(std::forward<std::unique_ptr<core::battle::Battle>>(battle))
, sequence(std::forward<std::unique_ptr<intros::SequenceBase>>(sequence))
, evolveIndex(0) {}

const char* BattleWrapperState::name() const { return "BattleWrapperState"; }

void BattleWrapperState::activate(bl::engine::Engine& engine) {
    engine.inputSystem().getActor().addListener(*this);

    switch (state) {
    case Substate::Battling:
        if (systems.player().state().evolutionPending()) {
            state       = Substate::Evolving;
            evolveIndex = 0;
            incEvolveIndex();
            startEvolve();
        }
        else { engine.popState(); }
        break;
    case Substate::Evolving:
        if (systems.player().state().evolutionPending()) {
            incEvolveIndex();
            startEvolve();
        }
        else { engine.popState(); }
        break;
    case Substate::BattleIntro:
    default:
        sequence->start(systems.engine());
        break;
    }
}

void BattleWrapperState::deactivate(bl::engine::Engine& engine) {
    engine.inputSystem().getActor().removeListener(*this);
}

void BattleWrapperState::update(bl::engine::Engine& engine, float dt, float) {
    sequence->update(dt);
    if (sequence->finished()) {
        state = Substate::Battling;
        engine.popState();
        // TODO - uncomment when done with intros
        // engine.pushState(BattleState::create(systems, std::move(battle)));
    }
}

bool BattleWrapperState::observe(const bl::input::Actor&, unsigned int, bl::input::DispatchType,
                                 bool) {
    return true;
}

void BattleWrapperState::incEvolveIndex() {
    const auto& team = systems.player().state().peoplemon;
    while (evolveIndex < team.size() && !team[evolveIndex].pendingEvolution()) { ++evolveIndex; }
}

void BattleWrapperState::startEvolve() {
    auto& ppl              = systems.player().state().peoplemon[evolveIndex];
    ppl.pendingEvolution() = false;
    systems.engine().pushState(Evolution::create(systems, ppl));
}

} // namespace state
} // namespace game
