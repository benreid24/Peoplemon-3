#include <Core/Battles/BattlerSubstate.hpp>

namespace core
{
namespace battle
{
BattlerSubstate::BattlerSubstate()
: lastMoveUsed(pplmn::MoveId::Unknown)
, isProtected(false)
, substituteHp(0) {}

void BattlerSubstate::notifyTurn(TurnAction action) {
    isProtected = false;
    if (action != TurnAction::Fight) { lastMoveUsed = pplmn::MoveId::Unknown; }
}

} // namespace battle
} // namespace core
