#include <Core/Systems/WildPeoplemon.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Battles/Battle.hpp>
#include <Core/Battles/BattleControllers/LocalBattleController.hpp>
#include <Core/Battles/BattlerControllers/AIController.hpp>
#include <Core/Maps/CatchRegion.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
const pplmn::WildPeoplemon DebugWild{
    .id = pplmn::Id::BenToo, .minLevel = 5, .maxLevel = 25, .frequency = 0};
}

WildPeoplemon::WildPeoplemon(Systems& o)
: owner(o) {}

void WildPeoplemon::init() { bl::event::Dispatcher::subscribe(this); }

void WildPeoplemon::startDebugBattle() {
    currentWild = DebugWild.generate();
    startBattle();
}

void WildPeoplemon::observe(const event::EntityMoveFinished& m) {
    if (m.entity == owner.player().player() && owner.player().state().repelSteps == 0) {
        const map::CatchRegion* region = owner.world().activeMap().getCatchRegion(m.position);
        if (region &&
            bl::util::Random::get<unsigned int>(0, 100) < Properties::WildPeoplemonChance()) {
            BL_LOG_INFO << "Starting wild battle";
            currentWild = region->selectWild().generate();

            startBattle();
        }
    }
}

void WildPeoplemon::startBattle() {
    std::unique_ptr<battle::Battle> battle =
        battle::Battle::create(owner.engine(),
                               owner.world().activeMap().getLocationName(owner.player().position()),
                               owner.player(),
                               battle::Battle::Type::WildPeoplemon);
    std::vector<pplmn::BattlePeoplemon> team;
    team.emplace_back(&currentWild);

    battle->state.enemy().init(
        std::move(team),
        std::make_unique<battle::AIController>(currentWild.name(), std::vector<item::Id>{}));
    battle->setController(std::make_unique<battle::LocalBattleController>());

    bl::event::Dispatcher::dispatch<event::BattleStarted>({std::move(battle)});
}

} // namespace system
} // namespace core
