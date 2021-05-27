#include <Core/Components/Controllable.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace component
{
Controllable::Controllable(system::Systems& systems, bl::entity::Entity owner)
: owner(owner)
, systems(systems)
, locked(false)
, wasLocked(false) {}

bool Controllable::processControl(Command ctrl, bool ignoreLock) {
    if (locked && !ignoreLock) return false;

    switch (ctrl) {
    case Command::MoveUp:
        return systems.movement().moveEntity(owner, Direction::Up, false);
    case Command::MoveRight:
        return systems.movement().moveEntity(owner, Direction::Right, false);
    case Command::MoveDown:
        return systems.movement().moveEntity(owner, Direction::Down, false);
    case Command::MoveLeft:
        return systems.movement().moveEntity(owner, Direction::Left, false);

    case Command::SprintUp:
        return systems.movement().moveEntity(owner, Direction::Up, true);
    case Command::SprintRight:
        return systems.movement().moveEntity(owner, Direction::Right, true);
    case Command::SprintDown:
        return systems.movement().moveEntity(owner, Direction::Down, true);
    case Command::SprintLeft:
        return systems.movement().moveEntity(owner, Direction::Left, true);

    case Command::Pause: // handled in PlayerControlled
    case Command::Back:
        return false;

    case Command::Interact:
        return systems.interaction().interact(owner);

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

} // namespace component
} // namespace core
