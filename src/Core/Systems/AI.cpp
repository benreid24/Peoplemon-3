#include <Core/Systems/AI.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
using StandingSet =
    bl::ecs::ComponentSet<component::StandingBehavior, component::Position, component::Movable>;
using SpinningSet =
    bl::ecs::ComponentSet<component::SpinBehavior, component::Position, component::Movable>;
using PathSet =
    bl::ecs::ComponentSet<component::FixedPathBehavior, component::Position, component::Movable>;
using WanderSet =
    bl::ecs::ComponentSet<component::WanderBehavior, component::Position, component::Movable>;
} // namespace

AI::AI(Systems& o)
: owner(o) {}

void AI::init() {
    standing = owner.engine()
                   .ecs()
                   .getOrCreateView<component::StandingBehavior,
                                    component::Position,
                                    component::Controllable>();
    spinning = owner.engine()
                   .ecs()
                   .getOrCreateView<component::SpinBehavior,
                                    component::Position,
                                    component::Controllable>();
    paths = owner.engine()
                .ecs()
                .getOrCreateView<component::FixedPathBehavior,
                                 component::Position,
                                 component::Controllable>();
    wandering = owner.engine()
                    .ecs()
                    .getOrCreateView<component::WanderBehavior,
                                     component::Position,
                                     component::Controllable>();
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

void AI::removeAi(bl::ecs::Entity ent) {
    owner.engine().ecs().removeComponent<component::StandingBehavior>(ent);
    owner.engine().ecs().removeComponent<component::SpinBehavior>(ent);
    owner.engine().ecs().removeComponent<component::FixedPathBehavior>(ent);
    owner.engine().ecs().removeComponent<component::WanderBehavior>(ent);
}

} // namespace system
} // namespace core
