#include <Game/Battles/Battle.hpp>

#include <Game/Battles/BattlerControllers/PlayerController.hpp>

namespace game
{
namespace battle
{
namespace
{
BattleState::Stage typeToStage(Battle::Type type) {
    switch (type) {
    case Battle::Type::WildPeoplemon:
        return BattleState::Stage::WildIntro;
    case Battle::Type::Trainer:
        return BattleState::Stage::TrainerIntro;
    case Battle::Type::Online:
        return BattleState::Stage::NetworkIntro;
    default:
        BL_LOG_ERROR << "Unknown battle type: " << type;
        return BattleState::Stage::IntroSendInSelf; // try to recover
    }
}
} // namespace

Battle::Battle(Type type)
: state(typeToStage(type)) {
    // custom init?
}

std::unique_ptr<Battle> Battle::create(core::system::Player& player, Type type) {
    std::unique_ptr<Battle> b(new Battle(type));

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
