#include <Game/Battles/BattleFSM.hpp>

namespace game
{
namespace battle
{
BattleFSM::BattleFSM()
: state(State::WaitingChoices) {}

void BattleFSM::init(Battler* p, Battler* o) {
    order[0] = p;
    order[1] = o;
    current  = 0;
}

bool BattleFSM::update(BattleState&) {
    // TODO - big-ass switch and lots of logic and state updates
    return false;
}

BattleFSM::State BattleFSM::currentState() const { return state; }

} // namespace battle
} // namespace game
