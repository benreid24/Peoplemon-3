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
, copyStatsFrom(-1)
, ailments(pplmn::PassiveAilment::None)
, turnsConfused(-1)
, turnsUntilAwake(-1)
, koReviveHp(-1)
, lastMoveHitWith(pplmn::MoveId::Unknown)
, lastDamageTaken(0)
, faintHandled(false)
, trainer(nullptr) {}

void BattlerSubstate::notifyTurnBegin() {
    isProtected     = false;
    noOneToGetBall  = false;
    move64Hit       = false;
    lastMoveHitWith = pplmn::MoveId::Unknown;
    lastDamageTaken = 0;
    faintHandled    = false;
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

    if (hasAilment(pplmn::PassiveAilment::Confused)) { turnsConfused -= 1; }
    else {
        turnsConfused = -1;
    }
    if (turnsUntilAwake > 0) { turnsUntilAwake -= 1; }
    clearAilment(pplmn::PassiveAilment::Distracted);
}

void BattlerSubstate::notifySwitch() {
    lastMoveIndex    = -1;
    substituteHp     = 0;
    chargingMove     = -1;
    lastMoveUsed     = pplmn::MoveId::Unknown;
    encoreTurnsLeft  = 0;
    encoreMove       = -1;
    deathCounter     = -1;
    enduringThisTurn = false;
    enduredLastTurn  = false;
    lastMoveHitWith  = pplmn::MoveId::Unknown;
    lastDamageTaken  = 0;
    turnsConfused    = -1;
    ailments         = pplmn::PassiveAilment::None;
    turnsUntilAwake  = -1;
    copyStatsFrom    = -1;
    koReviveHp       = -1;
}

void BattlerSubstate::giveAilment(pplmn::PassiveAilment ail) {
    using T  = std::underlying_type_t<pplmn::PassiveAilment>;
    ailments = static_cast<pplmn::PassiveAilment>(static_cast<T>(ail) | static_cast<T>(ailments));
}

bool BattlerSubstate::hasAilment(pplmn::PassiveAilment ail) const {
    using T = std::underlying_type_t<pplmn::PassiveAilment>;
    return (static_cast<T>(ailments) & static_cast<T>(ail)) != 0;
}

void BattlerSubstate::clearAilment(pplmn::PassiveAilment ail) {
    using T     = std::underlying_type_t<pplmn::PassiveAilment>;
    const T neg = ~static_cast<T>(ail);
    ailments    = static_cast<pplmn::PassiveAilment>(static_cast<T>(ailments) & neg);
}

} // namespace battle
} // namespace core
