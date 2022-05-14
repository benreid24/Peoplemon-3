#include <Core/Battles/BattleState.hpp>

namespace core
{
namespace battle
{
BattleState::BattleState(Stage state)
: stage(state)
, player(*this)
, opponent(*this)
, currentMover(0)
, firstMover(0) {}

void BattleState::beginRound(bool pf) {
    currentMover = pf ? 0 : 1;
    firstMover   = currentMover;
    player.notifyTurnBegin();
    opponent.notifyTurnBegin();
}

BattleState::Stage BattleState::nextTurn() {
    currentMover = (currentMover + 1) % 2;
    return currentMover == firstMover ? Stage::RoundEnd : Stage::TurnStart;
}

Battler& BattleState::localPlayer() { return player; }

Battler& BattleState::enemy() { return opponent; }

Battler& BattleState::activeBattler() { return currentMover == 0 ? player : opponent; }

Battler& BattleState::inactiveBattler() { return currentMover == 1 ? player : opponent; }

BattleState::Stage BattleState::currentStage() const { return stage; }

void BattleState::setStage(Stage s) { stage = s; }

bool BattleState::isFirstMover() const { return currentMover == firstMover; }

} // namespace battle
} // namespace core
