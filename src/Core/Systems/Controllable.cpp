#include <Core/Systems/Controllable.hpp>

#include <Core/Components/PlayerControlled.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Controllable::Controllable(Systems& owner)
: ecs(owner.engine().ecs()) {}

bool Controllable::setEntityLocked(bl::ecs::Entity e, bool l, bool p) {
    component::Controllable* c = ecs.getComponent<component::Controllable>(e);
    if (c) {
        BL_LOG_INFO << "Locked entity " << e;
        c->setLocked(l, p);
        return true;
    }
    else {
        BL_LOG_ERROR << "Failed to lock entity: " << e;
    }
    return false;
}

bool Controllable::resetEntityLock(bl::ecs::Entity e) {
    component::Controllable* c = ecs.getComponent<component::Controllable>(e);
    if (c) {
        c->resetLock();
        return true;
    }
    return false;
}

void Controllable::setAllLocks(bool l, bool p) {
    ecs.getAllComponents<component::Controllable>().forEach(
        [l, p](bl::ecs::Entity, component::Controllable& c) { c.setLocked(l, p); });
}

void Controllable::resetAllLocks() {
    ecs.getAllComponents<component::Controllable>().forEach(
        [](bl::ecs::Entity, component::Controllable& c) { c.resetLock(); });
}

} // namespace system
} // namespace core
