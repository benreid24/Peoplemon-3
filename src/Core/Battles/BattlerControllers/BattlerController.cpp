#include <Core/Battles/BattlerControllers/BattlerController.hpp>

namespace core
{
namespace battle
{
BattlerController::BattlerController()
: action(TurnAction::Fight)
, useItem(core::item::Id::None)
, subActionPicked(false) {}

void BattlerController::setOwner(Battler& o) { owner = &o; }

bool BattlerController::actionSelected() const { return subActionPicked; }

void BattlerController::pickAction() {
    subActionPicked = false;
    startChooseAction();
}

void BattlerController::pickPeoplemon(bool ff, bool ro) {
    subActionPicked = false;
    startChoosePeoplemon(ff, ro);
}

void BattlerController::chooseToContinue() {
    subActionPicked = false;
    startChooseToContinue();
}

bool BattlerController::shouldContinue() const { return dontGiveUp; }

TurnAction BattlerController::chosenAction() const { return action; }

int BattlerController::chosenMove() const { return move; }

core::item::Id BattlerController::chosenItem() const { return useItem; }

std::uint8_t BattlerController::chosenPeoplemon() const { return switchIndex; }

void BattlerController::chooseGiveUp(bool g) {
    dontGiveUp      = !g;
    subActionPicked = true;
}

void BattlerController::chooseMove(int m) {
    subActionPicked = true;
    move            = m;
    action          = TurnAction::Fight;
}

void BattlerController::choosePeoplemon(std::uint8_t i) {
    subActionPicked = true;
    switchIndex     = i;
    action          = TurnAction::Switch;
}

void BattlerController::chooseItem(std::uint8_t i, core::item::Id it) {
    subActionPicked = true;
    useItem         = it;
    switchIndex     = i;
    action          = TurnAction::Item;
}

void BattlerController::chooseRun() {
    action          = TurnAction::Run;
    subActionPicked = true;
}

} // namespace battle
} // namespace core
