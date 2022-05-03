#include <Core/Battles/BattlerControllers/PlayerController.hpp>

#include <Core/Battles/View/PlayerMenu.hpp>

namespace core
{
namespace battle
{
const std::string& PlayerController::name() const {
    static const std::string n = "<PLAYER>";
    return n;
}

PlayerController::PlayerController(view::PlayerMenu& menu)
: state(State::Waiting)
, menu(menu) {}

void PlayerController::refresh() {
    switch (state) {
    case State::PickingTurn:
        if (menu.ready()) {
            state = State::Waiting;
            switch (menu.selectedAction()) {
            case TurnAction::Fight:
                chooseMove(menu.selectedMove());
                break;
            case TurnAction::Item:
                chooseItem(menu.selectedItem());
                break;
            case TurnAction::Switch:
                choosePeoplemon(menu.selectedPeoplemon());
                break;
            case TurnAction::Run:
                chooseRun();
                break;
            default:
                break;
            }
        }
        break;

    case State::PickingFaintReplacement:
        if (menu.ready()) {
            state = State::Waiting;
            choosePeoplemon(menu.selectedPeoplemon());
        }
        break;

    default:
        break;
    }
}

void PlayerController::startChooseAction() {
    state = State::PickingTurn;
    menu.beginTurn();
}

void PlayerController::startChoosePeoplemon(bool fromFaint, bool reviveOnly) {
    state = State::PickingFaintReplacement;
    menu.choosePeoplemonMidTurn(fromFaint, reviveOnly);
}

} // namespace battle
} // namespace core
