#include <Core/Battles/BattleControllers/BattleController.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Battles/Battle.hpp>
#include <Core/Battles/BattleView.hpp>

namespace core
{
namespace battle
{
BattleController::BattleController()
: view(nullptr)
, state(nullptr)
, subState(SubState::Done) {}

void BattleController::init(Battle& b, BattleView& v, BattleState& s) {
    battle = &b;
    view   = &v;
    state  = &s;
}

void BattleController::update() {
    switch (subState) {
    case SubState::WaitingView:
        if (view->actionsCompleted()) {
            subState = SubState::Done;
            view->hideText();
            [[fallthrough]];
        }
        else {
            break;
        }

    case SubState::Done:
        if (!commandQueue.empty()) {
            while (updateCommandQueue()) {}
        }
        break;

    default:
        BL_LOG_WARN << "Unknown substate: " << subState;
        subState = SubState::Done;
        break;
    }

    onUpdate(subState == SubState::Done, commandQueue.empty());
}

void BattleController::queueCommand(Command&& cmd, bool addWait) {
    commandQueue.emplace(std::forward<Command>(cmd));
    onCommandQueued(commandQueue.back());
    if (addWait) {
        commandQueue.emplace(Command::WaitForView);
        onCommandQueued(commandQueue.back());
    }
}

bool BattleController::updateCommandQueue() {
    if (commandQueue.empty()) return false;

    const Command cmd = std::move(commandQueue.front());
    commandQueue.pop();
    onCommandProcessed(cmd);

    switch (cmd.getType()) {
    case Command::Type::DisplayMessage:
    case Command::Type::PlayAnimation:
    case Command::Type::SyncStateNoSwitch:
    case Command::Type::SyncStatePlayerSwitch:
    case Command::Type::SyncStateOpponentSwitch:
        view->processCommand(cmd);
        return true;

    case Command::Type::WaitForView:
        subState = SubState::WaitingView;
        return false;

    case Command::Type::GetBattlerChoices:
        subState = SubState::WaitingView;
        state->localPlayer().pickAction();
        state->enemy().pickAction();
        return false;

    case Command::Type::GetMidTurnSwitch:
    case Command::Type::GetFaintSwitch: {
        Battler& b = cmd.forActiveBattler() ? state->activeBattler() : state->inactiveBattler();
        subState   = SubState::WaitingView;
        b.pickPeoplemon(cmd.getType() == Command::GetFaintSwitch, b.getSubstate().koReviveHp > 0);
        return false;
    }

    default:
        BL_LOG_WARN << "Unknown command type: " << cmd.getType();
        return true;
    }
}

} // namespace battle
} // namespace core
