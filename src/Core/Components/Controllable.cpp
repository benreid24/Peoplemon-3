#include <Core/Components/Controllable.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace component
{
Controllable::Controllable(system::Systems& systems, bl::ecs::Entity owner)
: owner(owner)
, systems(systems)
, locked(false)
, wasLocked(false) {}

bool Controllable::processControl(input::EntityControl ctrl, bool sprint, bool ignoreLock) {
    if (locked && !ignoreLock) return false;

    switch (ctrl) {
    case input::Control::MoveUp:
        return systems.movement().moveEntity(owner, bl::tmap::Direction::Up, sprint);
    case input::Control::MoveRight:
        return systems.movement().moveEntity(owner, bl::tmap::Direction::Right, sprint);
    case input::Control::MoveDown:
        return systems.movement().moveEntity(owner, bl::tmap::Direction::Down, sprint);
    case input::Control::MoveLeft:
        return systems.movement().moveEntity(owner, bl::tmap::Direction::Left, sprint);

    case input::Control::Pause: // handled in PlayerControlled
    case input::Control::Back:
        return false;

    case input::Control::Interact:
        return systems.interaction().interact(owner);

    case input::Control::Sprint:
        return false;

    default:
        BL_LOG_WARN << "Unknown control: " << static_cast<int>(ctrl);
        return false;
    }
}

void Controllable::setLocked(bool l, bool p) {
    if (p) wasLocked = locked;
    locked = l;
}

void Controllable::resetLock() { locked = wasLocked; }

bool Controllable::isLocked() const { return locked; }

} // namespace component
} // namespace core
