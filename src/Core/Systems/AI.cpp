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
    standing = owner.engine().entities().getEntitiesWithComponents<component::StandingBehavior>();
}

void AI::update(float dt) {
    const auto updateEntity = [this](bl::entity::Entity entity) {
        auto stand = standing->results().find(entity);
        if (stand != standing->results().end()) {
            stand->second.get<component::StandingBehavior>()->update();
        }

        // others
    };

    for (bl::entity::Entity e : owner.position().updateRangeEntities()) { updateEntity(e); }
}

bool AI::addBehavior(bl::entity::Entity e, file::Behavior behvaior) {
    // TODO - make behavior into struct with data
    switch (behvaior) {
    case file::Behavior::StandStill:
        return makeStanding(e, component::Direction::Up);
    default:
        return false;
    }
}

bool AI::makeStanding(bl::entity::Entity e, component::Direction dir) {
    auto posHandle = owner.engine().entities().getComponentHandle<component::Position>(e);
    if (!posHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get position for entity: " << e;
        return false;
    }

    auto controlHandle = owner.engine().entities().getComponentHandle<component::Controllable>(e);
    if (!controlHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get controllable for entity: " << e;
        return false;
    }

    if (!owner.engine().entities().addComponent<component::StandingBehavior>(
            e, {dir, posHandle, controlHandle})) {
        BL_LOG_ERROR << "Failed to add standing behavior to entity: " << e;
        return false;
    }

    return true;
}

} // namespace system
} // namespace core
