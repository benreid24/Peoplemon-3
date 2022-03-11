#include <Core/Systems/AI.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
AI::AI(Systems& o)
: owner(o) {}

void AI::init() {
    standing = owner.engine()
                   .entities()
                   .getEntitiesWithComponents<component::StandingBehavior,
                                              component::Position,
                                              component::Controllable>();
    spinning = owner.engine()
                   .entities()
                   .getEntitiesWithComponents<component::SpinBehavior,
                                              component::Position,
                                              component::Controllable>();
    paths = owner.engine()
                .entities()
                .getEntitiesWithComponents<component::FixedPathBehavior,
                                           component::Position,
                                           component::Controllable>();
    wandering = owner.engine()
                    .entities()
                    .getEntitiesWithComponents<component::WanderBehavior,
                                               component::Position,
                                               component::Controllable>();
}

void AI::update(float dt) {
    const auto updateEntity = [this, dt](bl::entity::Entity entity) {
        auto stand = standing->results().find(entity);
        if (stand != standing->results().end()) {
            stand->second.get<component::StandingBehavior>()->update(
                *stand->second.get<component::Position>(),
                *stand->second.get<component::Controllable>());
        }

        auto spin = spinning->results().find(entity);
        if (spin != spinning->results().end()) {
            spin->second.get<component::SpinBehavior>()->update(
                *spin->second.get<component::Position>(),
                *spin->second.get<component::Controllable>(),
                dt);
        }

        auto path = paths->results().find(entity);
        if (path != paths->results().end()) {
            path->second.get<component::FixedPathBehavior>()->update(
                *path->second.get<component::Position>(),
                *path->second.get<component::Controllable>());
        }

        auto wander = wandering->results().find(entity);
        if (wander != wandering->results().end()) {
            wander->second.get<component::WanderBehavior>()->update(
                *wander->second.get<component::Position>(),
                *wander->second.get<component::Controllable>(),
                dt);
        }
    };

    for (bl::entity::Entity e : owner.position().updateRangeEntities()) { updateEntity(e); }
}

bool AI::addBehavior(bl::entity::Entity e, const file::Behavior& behavior) {
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

bool AI::makeStanding(bl::entity::Entity e, component::Direction dir) {
    if (!owner.engine().entities().addComponent<component::StandingBehavior>(e, {dir})) {
        BL_LOG_ERROR << "Failed to add standing behavior to entity: " << e;
        return false;
    }

    return true;
}

bool AI::makeSpinning(bl::entity::Entity e, file::Behavior::Spinning::Direction dir) {
    if (!owner.engine().entities().addComponent<component::SpinBehavior>(e, {dir})) {
        BL_LOG_ERROR << "Failed to add spinning behavior to entity: " << e;
        return false;
    }
    return true;
}

bool AI::makeFollowPath(bl::entity::Entity e, const file::Behavior::Path& path) {
    if (!owner.engine().entities().addComponent<component::FixedPathBehavior>(e, {path})) {
        BL_LOG_ERROR << "Failed to add fixed path behavior to entity: " << e;
        return false;
    }
    return true;
}

bool AI::makeWander(bl::entity::Entity e, unsigned int radius) {
    const component::Position* pos = owner.engine().entities().getComponent<component::Position>(e);
    if (!pos) {
        BL_LOG_ERROR << "Cannot add wander behavior to entity (" << e
                     << ") without position component";
        return false;
    }

    if (!owner.engine().entities().addComponent<component::WanderBehavior>(
            e, {radius, pos->positionTiles()})) {
        BL_LOG_ERROR << "Failed to add wander behavior to entity: " << e;
        return false;
    }
    return true;
}

void AI::removeAi(bl::entity::Entity ent) {
    if (owner.engine().entities().removeComponent<component::StandingBehavior>(ent)) return;
    if (owner.engine().entities().removeComponent<component::SpinBehavior>(ent)) return;
    if (owner.engine().entities().removeComponent<component::FixedPathBehavior>(ent)) return;
    owner.engine().entities().removeComponent<component::WanderBehavior>(ent);
}

} // namespace system
} // namespace core
