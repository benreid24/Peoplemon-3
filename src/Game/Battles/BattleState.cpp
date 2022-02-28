#include <Game/Battles/BattleState.hpp>

namespace game
{
namespace battle
{
BattleState::BattleState()
: stage(Stage::Intro)
, player(*this)
, opponent(*this)
, currentMover(0) {}

Battler& BattleState::localPlayer() { return player; }

Battler& BattleState::enemy() { return opponent; }

Battler& BattleState::activeBattler() { return currentMover == 0 ? player : opponent; }

BattleState::Stage BattleState::currentStage() const { return stage; }

} // namespace battle
} // namespace game
