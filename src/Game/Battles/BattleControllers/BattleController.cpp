#include <Game/Battles/BattleControllers/BattleController.hpp>

#include <BLIB/Logging.hpp>

namespace game
{
namespace battle
{
BattleController::BattleController()
: view(nullptr)
, state(nullptr)
, subState(SubState::Done) {}

void BattleController::init(BattleView& v, BattleState& s) {
    view  = &v;
    state = &s;
}

void BattleController::update() {
    switch (subState) {
    case SubState::WaitingView:
        // TODO - query view and update if done
        break;

    case SubState::Done:
        if (!commandQueue.empty()) {
            while (updateCommandQueue()) {}
        }
        break;

    default:
        BL_LOG_WARN << "Unknown substate: " << subState;
        break;
    }

    onUpdate();
}

void BattleController::queueCommand(Command&& cmd) {
    commandQueue.emplace(std::forward<Command>(cmd));
    onCommandQueued(commandQueue.back());
}

bool BattleController::updateCommandQueue() {
    if (commandQueue.empty()) return false;

    const Command& cmd = commandQueue.front();
    bool result        = false;

    switch (cmd.getType()) {
    case Command::Type::DisplayMessage:
        // TODO - send to view
        subState = SubState::WaitingView;
        result   = true; // we can queue up view actions
        break;

    case Command::Type::PlayAnimation:
        // TODO - send to view
        subState = SubState::WaitingView;
        result   = true; // we can queue up view actions
        break;

    case Command::Type::SyncState:
        // TODO - update view state
        subState = SubState::WaitingView; // wait for bars
        result   = true;                  // we can queue up view actions
        break;

    default:
        BL_LOG_WARN << "Unknown command type: " << cmd.getType();
        commandQueue.pop();
        return true;
    }

    onCommandProcessed(cmd);
    commandQueue.pop();
    return result;
}

} // namespace battle
} // namespace game
