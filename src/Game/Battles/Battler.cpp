#include <Game/Battles/Battle.hpp>

#include <Game/Battles/BattleState.hpp>

namespace game
{
namespace battle
{
Battler::Battler(BattleState& state)
: state(state)
, currentPeoplemon(0) {}

void Battler::init(std::vector<core::pplmn::BattlePeoplemon>&& t,
                   std::unique_ptr<BattlerController>& c) {
    team = t;
    controller.swap(c);
}

bool Battler::actionSelected() const { return controller->actionSelected(); }

void Battler::pickAction() { controller->pickAction(); }

void Battler::pickPeoplemon() { controller->pickPeoplemon(); }

TurnAction Battler::chosenAction() const { return controller->chosenAction(); }

core::pplmn::MoveId Battler::chosenMove() const { return controller->chosenMove(); }

core::item::Id Battler::chosenItem() const { return controller->chosenItem(); }

std::uint8_t Battler::chosenPeoplemon() const { return controller->chosenPeoplemon(); }

std::vector<core::pplmn::BattlePeoplemon>& Battler::peoplemon() { return team; }

core::pplmn::BattlePeoplemon& Battler::activePeoplemon() { return team[currentPeoplemon]; }

bool Battler::canFight() const {
    for (const auto& ppl : team) {
        if (ppl.base().currentHp() > 0) return true;
    }
    return false;
}

} // namespace battle
} // namespace game
