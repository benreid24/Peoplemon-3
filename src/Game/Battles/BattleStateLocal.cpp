#include <Game/Battles/BattleStateLocal.hpp>

namespace game
{
namespace battle
{
BattleStateLocal::BattleStateLocal(core::system::Player& p, std::unique_ptr<Battler>&& op)
: BattleState()
, notPlayer(std::forward<std::unique_ptr<Battler>>(op)) {
    // TODO - player battler
    opponent = notPlayer.get();
}

void BattleStateLocal::update() {
    if (updateCommandQueue()) {
        while (fsm.update(*this)) {}
    }
}

} // namespace battle
} // namespace game
