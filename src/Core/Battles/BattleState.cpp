#include <Core/Battles/BattleState.hpp>

#include <Core/Battles/BattleSkipper.hpp>

namespace core
{
namespace battle
{
BattleState::BattleState(Stage state)
: stage(state)
, player(*this)
, opponent(*this)
, currentMover(0) {
#ifdef PEOPLEMON_DEBUG
    if (BattleSkipper::skipBattles()) { stage = Stage::Victory; }
#endif
}

Battler& BattleState::localPlayer() { return player; }

Battler& BattleState::enemy() { return opponent; }

Battler& BattleState::activeBattler() { return currentMover == 0 ? player : opponent; }

BattleState::Stage BattleState::currentStage() const { return stage; }

} // namespace battle
} // namespace core
