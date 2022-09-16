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
WildPeoplemon::WildPeoplemon(Systems& o)
: owner(o) {}

void WildPeoplemon::init() { owner.engine().eventBus().subscribe(this); }

void WildPeoplemon::observe(const event::EntityMoveFinished& m) {
    if (m.entity == owner.player().player() && owner.player().state().repelSteps == 0) {
        const map::CatchRegion* region = owner.world().activeMap().getCatchRegion(m.position);
        if (region &&
            bl::util::Random::get<unsigned int>(0, 100) < Properties::WildPeoplemonChance()) {
            BL_LOG_INFO << "Starting wild battle";
            currentWild = region->selectWild().generate();

            std::unique_ptr<battle::Battle> battle = battle::Battle::create(
                owner.player(), battle::Battle::Type::WildPeoplemon, owner.engine().eventBus());
            std::vector<pplmn::BattlePeoplemon> team;
            team.emplace_back(&currentWild);

            battle->state.enemy().init(std::move(team),
                                       std::make_unique<battle::AIController>(
                                           currentWild.name(), std::vector<item::Id>{}));
            battle->setController(std::make_unique<battle::LocalBattleController>());

            owner.engine().eventBus().dispatch<event::BattleStarted>({std::move(battle)});
        }
    }
}

} // namespace system
} // namespace core
