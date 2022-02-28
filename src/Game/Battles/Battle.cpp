#include <Game/Battles/Battle.hpp>

#include <Game/Battles/BattlerControllers/PlayerController.hpp>

namespace game
{
namespace battle
{
Battle::Battle() {
    // custom init?
}

std::unique_ptr<Battle> Battle::create(core::system::Player& player) {
    std::unique_ptr<Battle> b(new Battle());

    std::unique_ptr<BattlerController> c = std::make_unique<PlayerController>();
    std::vector<core::pplmn::BattlePeoplemon> team;
    team.reserve(player.team().size());
    for (auto& ppl : player.team()) { team.emplace_back(&ppl); }
    b->state.localPlayer().init(std::move(team), c);

    return b;
}

void Battle::setController(std::unique_ptr<BattleController>& c) { controller.swap(c); }

} // namespace battle
} // namespace game
