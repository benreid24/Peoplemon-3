#include <Core/Systems/AI.hpp>

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

} // namespace system
} // namespace core
