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

void BattleController::queueCommand(Command&& cmd) {
    commandQueue.emplace(std::forward<Command>(cmd));
    onCommandQueued(commandQueue.back());
}

bool BattleController::updateCommandQueue() {
    if (commandQueue.empty()) return false;

    const Command& cmd = commandQueue.front();
    switch (cmd.getType()) {
    case Command::Type::DisplayMessage:
    case Command::Type::PlayAnimation:
    case Command::Type::SyncStateNoSwitch:
    case Command::Type::SyncStatePlayerSwitch:
    case Command::Type::SyncStateOpponentSwitch:
        view->processCommand(cmd);
        subState = SubState::WaitingView;
        break;

    default:
        BL_LOG_WARN << "Unknown command type: " << cmd.getType();
        commandQueue.pop();
        return true;
    }

    const bool wait = cmd.waitForView();
    onCommandProcessed(cmd);
    commandQueue.pop();
    return !wait;
}

} // namespace battle
} // namespace core
