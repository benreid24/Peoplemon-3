#include <Core/Battles/Battle.hpp>

#include <Core/Battles/BattleSkipper.hpp>
#include <Core/Battles/BattlerControllers/PlayerController.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace battle
{
namespace
{
BattleState::Stage typeToStage(Battle::Type type) {
#ifdef PEOPLEMON_DEBUG
    const bool skip = BattleSkipper::skipBattles();
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

Battle::Battle(system::Player& player, Type type, bl::event::Dispatcher& eb)
: player(player)
, type(type)
, state(typeToStage(type))
, view(state, type == Type::WildPeoplemon, eb)
, localPlayerWon(false) {
    // custom init?
}

std::unique_ptr<Battle> Battle::create(system::Player& player, Type type,
                                       bl::event::Dispatcher& eb) {
    std::unique_ptr<Battle> b(new Battle(player, type, eb));

    std::vector<pplmn::BattlePeoplemon> team;
    team.reserve(player.team().size());
    for (auto& ppl : player.team()) { team.emplace_back(&ppl); }
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
