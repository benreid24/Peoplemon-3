#include <Core/Battles/BattlerSubstate.hpp>

namespace core
{
namespace battle
{
BattlerSubstate::BattlerSubstate()
: lastMoveUsed(pplmn::MoveId::Unknown)
, isProtected(false)
, substituteHp(0)
, turnsGuarded(0)
, chargingMove(-1) {}

void BattlerSubstate::notifyTurn(TurnAction action) {
    isProtected = false;
    if (action != TurnAction::Fight) { lastMoveUsed = pplmn::MoveId::Unknown; }
    if (turnsGuarded > 0) { turnsGuarded -= 1; }
}

void BattlerSubstate::notifySwitch(bool fainted) {
    substituteHp = 0;
    chargingMove = -1;
    lastMoveUsed = pplmn::MoveId::Unknown;
}

} // namespace battle
} // namespace core
