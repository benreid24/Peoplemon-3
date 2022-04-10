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

Battler& BattleState::localPlayer() { return player; }

Battler& BattleState::enemy() { return opponent; }

Battler& BattleState::activeBattler() { return currentMover == 0 ? player : opponent; }

BattleState::Stage BattleState::currentStage() const { return stage; }

void BattleState::setStage(Stage s) {
    stage = s;
    // TODO - send sync event?
}

} // namespace battle
} // namespace core
