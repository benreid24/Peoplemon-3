#include <Game/Battles/BattleState.hpp>

namespace game
{
namespace battle
{
BattleState::BattleState()
: subState(SubState::Done)
, player(nullptr)
, opponent(nullptr) {}

void BattleState::consumeCommand(Command&& cmd) { processCommand(std::forward<Command>(cmd)); }

void BattleState::processCommand(Command&& cmd) {
    commandQueue.emplace(cmd);
    if (subState == SubState::Done) {
        // TODO - how to communicate prints and anims? maybe have an interface class that takes
        // those and applies them and manages UI states and transient states
    }
}

Battler& BattleState::localPlayer() { return *player; }

Battler& BattleState::enemy() { return *opponent; }

bool BattleState::updateCommandQueue() {
    // TODO - process commands from the queue and update substate
    return subState == SubState::Done;
}

} // namespace battle
} // namespace game
