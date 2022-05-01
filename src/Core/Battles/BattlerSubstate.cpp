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
, chargingMove(-1)
, encoreMove(-1)
, encoreTurnsLeft(0)
, deathCounter(-1)
, ballBlocked(false)
, ballBumped(false)
, ballSet(false)
, ballSpiked(false)
, ballSwiped(false)
, noOneToGetBall(false)
, ballUpTurns(0) {}

void BattlerSubstate::notifyTurnBegin() {
    isProtected    = false;
    noOneToGetBall = false;
}

void BattlerSubstate::notifyTurnEnd(TurnAction action) {
    if (action != TurnAction::Fight) { lastMoveUsed = pplmn::MoveId::Unknown; }
    if (turnsGuarded > 0) { turnsGuarded -= 1; }
    if (encoreTurnsLeft > 0) {
        encoreTurnsLeft -= 1;
        if (encoreTurnsLeft == 0) { encoreMove = -1; }
    }
    if (deathCounter > 0) { deathCounter -= 1; }
    if (ballUpTurns >= 0) { ballUpTurns += 1; }
}

void BattlerSubstate::notifySwitch(bool fainted) {
    substituteHp    = 0;
    chargingMove    = -1;
    lastMoveUsed    = pplmn::MoveId::Unknown;
    encoreTurnsLeft = 0;
    encoreMove      = -1;
    deathCounter    = -1;
}

} // namespace battle
} // namespace core
