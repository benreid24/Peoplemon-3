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
