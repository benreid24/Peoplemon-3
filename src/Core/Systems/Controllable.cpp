#include <Core/Systems/Controllable.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Controllable::Controllable(Systems& owner)
: owner(owner) {}

void Controllable::init() {
    entities = owner.engine().entities().getEntitiesWithComponents<component::Controllable>();
}

bool Controllable::setEntityLocked(bl::entity::Entity e, bool l, bool p) {
    auto it = entities->results().find(e);
    if (it != entities->results().end()) {
        it->second.get<component::Controllable>()->setLocked(l, p);
        return true;
    }
    return false;
}

bool Controllable::resetEntityLock(bl::entity::Entity e) {
    auto it = entities->results().find(e);
    if (it != entities->results().end()) {
        it->second.get<component::Controllable>()->resetLock();
        return true;
    }
    return false;
}

void Controllable::setAllLocks(bool l, bool p) {
    for (auto& pair : entities->results()) {
        pair.second.get<component::Controllable>()->setLocked(l, p);
    }
}

void Controllable::resetAllLocks() {
    for (auto& pair : entities->results()) {
        pair.second.get<component::Controllable>()->resetLock();
    }
}

} // namespace system
} // namespace core
