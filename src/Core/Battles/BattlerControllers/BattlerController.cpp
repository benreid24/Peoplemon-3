#include <Core/Battles/BattlerControllers/BattlerController.hpp>

namespace core
{
namespace battle
{
BattlerController::BattlerController()
: action(TurnAction::Fight)
, useItem(core::item::Id::None)
, switchIndex(0)
, move(-1)
, actionChoosed(false)
, subActionPicked(false) {}

bool BattlerController::actionSelected() const { return actionChoosed; }

void BattlerController::pickAction() {
    actionChoosed   = false;
    subActionPicked = false;
    startChooseAction();
}

void BattlerController::pickPeoplemon() {
    actionChoosed   = false;
    subActionPicked = false;
    startChoosePeoplemon();
}

TurnAction BattlerController::chosenAction() const { return action; }

int BattlerController::chosenMove() const { return move; }

core::item::Id BattlerController::chosenItem() const { return useItem; }

std::uint8_t BattlerController::chosenPeoplemon() const { return switchIndex; }

void BattlerController::chooseAction(TurnAction a) {
    actionChoosed = true;
    action        = a;
}

void BattlerController::chooseMove(int m) {
    subActionPicked = true;
    move            = m;
}

void BattlerController::choosePeoplemon(std::uint8_t i) {
    subActionPicked = true;
    switchIndex     = i;
}

void BattlerController::chooseItem(core::item::Id it) {
    subActionPicked = true;
    useItem         = it;
}

} // namespace battle
} // namespace core
