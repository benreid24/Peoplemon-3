#include <Core/Systems/AI.hpp>

#include <BLIB/AI/PathFinder.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Math.hpp>
#include <BLIB/Util/Hashes.hpp>
#include <Core/Events/PathFind.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
struct PositionHash {
    std::size_t operator()(const component::Position& pos) const {
        const std::size_t tileHash = bl::util::Vector2Hash<int>()(pos.positionTiles());
        return bl::util::hashCombine(tileHash, pos.level);
    }
};

using PathFinder = bl::ai::PathFinder<component::Position, PositionHash>;
} // namespace

AI::AI(Systems& o)
: owner(o) {}

void AI::init() {
    standing  = owner.engine().ecs().getOrCreateView<StandingTypes>();
    spinning  = owner.engine().ecs().getOrCreateView<SpinTypes>();
    paths     = owner.engine().ecs().getOrCreateView<FixedPathTypes>();
    wandering = owner.engine().ecs().getOrCreateView<WanderTypes>();
}

void AI::update(float dt) {
    standing->forEach([](StandingRow& cs) {
        cs.get<component::StandingBehavior>()->update(*cs.get<component::Position>(),
                                                      *cs.get<component::Controllable>());
    });

    spinning->forEach([dt](SpinRow& cs) {
        cs.get<component::SpinBehavior>()->update(
            *cs.get<component::Position>(), *cs.get<component::Controllable>(), dt);
    });

    paths->forEach([](FixedPathRow& cs) {
        cs.get<component::FixedPathBehavior>()->update(*cs.get<component::Position>(),
                                                       *cs.get<component::Controllable>());
    });

    wandering->forEach([dt](WanderRow& cs) {
        cs.get<component::WanderBehavior>()->update(
            *cs.get<component::Position>(), *cs.get<component::Controllable>(), dt);
    });

    auto& ecs = owner.engine().ecs();
    ecs.getAllComponents<component::PathFinder>().forEachWithWrites(
        [this, &ecs](bl::ecs::Entity entity, component::PathFinder& path) {
            const auto cleanup = [this, entity, &ecs]() {
                ecs.removeComponent<component::PathFinder>(entity);
                owner.controllable().resetEntityLock(entity);
            };

            auto cs =
                ecs.getComponentSet<bl::ecs::Require<component::Position, component::Movable>>(
                    entity);
            if (!cs.isValid()) {
                BL_LOG_ERROR << "Cannot pathfind entity without Movable/Position component: "
                             << entity;
                cleanup();
                return;
            }

            if (!cs.get<component::Movable>()->moving()) {
                if (path.step == path.path.size()) {
                    if (cs.get<component::Position>()->direction != path.destination.direction) {
                        owner.movement().moveEntity(entity, path.destination.direction, false);
                    }
                    cleanup();
                    bl::event::Dispatcher::dispatch<event::PathFindCompleted>({entity, true});
                }
                else {
                    component::Position& cpos       = *cs.get<component::Position>();
                    const component::Position& npos = path.path[path.step];
                    ++path.step;
                    cpos.direction = component::Position::facePosition(cpos, npos);
                    if (!owner.movement().moveEntity(entity, cpos.direction, false)) {
                        BL_LOG_INFO << "Entity path blocked, recomputing";
                        if (!findPath(entity, path)) {
                            BL_LOG_ERROR << "Failed to find new path, aborting";
                            cleanup();
                        }
                        // continue and let the motion start on the next frame
                    }
                }
            }
        });
}

bool AI::addBehavior(bl::ecs::Entity e, const file::Behavior& behavior) {
    switch (behavior.type()) {
    case file::Behavior::StandStill:
        return makeStanding(e, behavior.standing().facedir);
    case file::Behavior::SpinInPlace:
        return makeSpinning(e, behavior.spinning().spinDir);
    case file::Behavior::FollowingPath:
        return makeFollowPath(e, behavior.path());
    case file::Behavior::Wandering:
        return makeWander(e, behavior.wander().radius);
    default:
        return false;
    }
}

bool AI::makeStanding(bl::ecs::Entity e, component::Direction dir) {
    removeAi(e);

    if (!owner.engine().ecs().addComponent<component::StandingBehavior>(e, {dir})) {
        BL_LOG_ERROR << "Failed to add standing behavior to entity: " << e;
        return false;
    }

    return true;
}

bool AI::makeSpinning(bl::ecs::Entity e, file::Behavior::Spinning::Direction dir) {
    removeAi(e);

    if (!owner.engine().ecs().addComponent<component::SpinBehavior>(e, {dir})) {
        BL_LOG_ERROR << "Failed to add spinning behavior to entity: " << e;
        return false;
    }
    return true;
}

bool AI::makeFollowPath(bl::ecs::Entity e, const file::Behavior::Path& path) {
    removeAi(e);

    if (!owner.engine().ecs().addComponent<component::FixedPathBehavior>(e, {path})) {
        BL_LOG_ERROR << "Failed to add fixed path behavior to entity: " << e;
        return false;
    }
    return true;
}

bool AI::makeWander(bl::ecs::Entity e, unsigned int radius) {
    removeAi(e);

    const component::Position* pos = owner.engine().ecs().getComponent<component::Position>(e);
    if (!pos) {
        BL_LOG_ERROR << "Cannot add wander behavior to entity (" << e
                     << ") without position component";
        return false;
    }

    if (!owner.engine().ecs().addComponent<component::WanderBehavior>(
            e, {radius, pos->positionTiles()})) {
        BL_LOG_ERROR << "Failed to add wander behavior to entity: " << e;
        return false;
    }
    return true;
}

bool AI::moveToPosition(bl::ecs::Entity entity, const component::Position& dest) {
    component::Controllable* ctrl =
        owner.engine().ecs().getComponent<component::Controllable>(entity);
    if (ctrl) { ctrl->setLocked(true); }

    component::PathFinder path(dest);
    if (!findPath(entity, path)) {
        if (ctrl) { ctrl->resetLock(); }
        return false;
    }
    owner.engine().ecs().emplaceComponent<component::PathFinder>(entity, std::move(path));
    return true;
}

void AI::removeAi(bl::ecs::Entity ent) {
    owner.engine().ecs().removeComponent<component::StandingBehavior>(ent);
    owner.engine().ecs().removeComponent<component::SpinBehavior>(ent);
    owner.engine().ecs().removeComponent<component::FixedPathBehavior>(ent);
    owner.engine().ecs().removeComponent<component::WanderBehavior>(ent);
}

bool AI::findPath(bl::ecs::Entity ent, component::PathFinder& path) {
    component::Position* start = owner.engine().ecs().getComponent<component::Position>(ent);
    if (!start) {
        BL_LOG_ERROR << "Tried to pathfind an entity without a position: " << ent;
        return false;
    }

    const map::Map& cmap = owner.world().activeMap();
    const Position& psys = owner.position();

    const auto getAdjacent = [&cmap,
                              &psys](const component::Position& pos,
                                     std::vector<std::pair<component::Position, int>>& adjacent) {
        const std::array<component::Direction, 4> dirs{component::Direction::Up,
                                                       component::Direction::Right,
                                                       component::Direction::Down,
                                                       component::Direction::Left};
        component::Position tpos(pos.level, pos.positionTiles(), pos.direction);
        ;
        for (const component::Direction dir : dirs) {
            tpos.direction = dir;
            if (!cmap.movePossible(tpos, dir)) continue;
            const component::Position apos = cmap.adjacentTile(tpos, dir);
            if (!cmap.contains(apos)) continue;
            if (!psys.spaceFree(apos)) continue;
            adjacent.emplace_back(apos, 1);
        }
    };

    const auto estDistance = [](const component::Position& p1,
                                const component::Position& p2) -> int {
        const int tdist = bl::math::manhattanDistance(p1.positionTiles(), p2.positionTiles());
        return tdist + (p1.level >= p2.level ? p1.level - p2.level : p2.level - p1.level);
    };

    path.step = 0;
    return PathFinder::findPath(*start, path.destination, getAdjacent, estDistance, path.path);
}

} // namespace system
} // namespace core
