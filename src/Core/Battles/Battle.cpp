#include <Core/Battles/Battle.hpp>

#include <Core/Battles/BattlerControllers/PlayerController.hpp>
#include <Core/Debug/DebugOverrides.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace battle
{
namespace
{
BattleState::Stage typeToStage(Battle::Type type) {
#ifdef PEOPLEMON_DEBUG
    const bool skip = debug::DebugOverrides::get().skipBattles;
#else
    const bool skip = false;
#endif

    switch (type) {
    case Battle::Type::WildPeoplemon:
        return skip ? BattleState::Stage::Completed : BattleState::Stage::WildIntro;
    case Battle::Type::Trainer:
        return skip ? BattleState::Stage::TrainerDefeated : BattleState::Stage::TrainerIntro;
    case Battle::Type::Online:
        return BattleState::Stage::NetworkIntro;
    default:
        BL_LOG_ERROR << "Unknown battle type: " << type;
        return BattleState::Stage::IntroSendInSelf; // try to recover
    }
}
} // namespace

Battle::Battle(const std::string& location, system::Player& player, Type type)
: location(location)
, player(player)
, type(type)
, state(typeToStage(type))
, view(state, type == Type::WildPeoplemon)
, localPlayerWon(false) {}

std::unique_ptr<Battle> Battle::create(const std::string& location, system::Player& player,
                                       Type type) {
    std::unique_ptr<Battle> b(new Battle(location, player, type));

    std::vector<pplmn::BattlePeoplemon> team;
    team.reserve(player.state().peoplemon.size());
    for (auto& ppl : player.state().peoplemon) { team.emplace_back(&ppl); }
    b->state.localPlayer().init(std::move(team),
                                std::make_unique<PlayerController>(player, b->view.menu()));

    return b;
}

void Battle::setController(std::unique_ptr<BattleController>&& c) {
    controller.swap(c);
    controller->init(*this, view, state);
}

} // namespace battle
} // namespace core
