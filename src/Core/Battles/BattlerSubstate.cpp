#include <Core/Battles/BattlerSubstate.hpp>

namespace core
{
namespace battle
{
BattlerSubstate::BattlerSubstate()
: lastMoveIndex(-1)
, lastMoveUsed(pplmn::MoveId::Unknown)
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
, ballUpTurns(0)
, enduringThisTurn(false)
, enduredLastTurn(false)
, spikesOut(0)
, healNext(-1)
, move64Hit(false)
, copyStatsFrom(-1) {}

void BattlerSubstate::notifyTurnBegin() {
    isProtected    = false;
    noOneToGetBall = false;
    move64Hit      = false;
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
    enduredLastTurn  = enduringThisTurn;
    enduringThisTurn = false;
    if (healNext >= 0) { healNext += 1; }
}

void BattlerSubstate::notifySwitch(bool fainted) {
    lastMoveIndex    = -1;
    substituteHp     = 0;
    chargingMove     = -1;
    lastMoveUsed     = pplmn::MoveId::Unknown;
    encoreTurnsLeft  = 0;
    encoreMove       = -1;
    deathCounter     = -1;
    enduringThisTurn = false;
    enduredLastTurn  = false;
}

} // namespace battle
} // namespace core
