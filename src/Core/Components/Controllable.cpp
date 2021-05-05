#include <Core/Components/Controllable.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace component
{
Controllable::Controllable(system::Systems& systems, bl::entity::Entity owner)
: owner(owner)
, systems(systems) {}

bool Controllable::processControl(Command ctrl) {
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

    case Command::Pause:
        // TODO - pause menu
        return false;

    case Command::Back:
        // TODO - back menu?
        return false;

    case Command::Interact:
        // TODO - interaction system
        return false;

    default:
        BL_LOG_WARN << "Unknown control: " << static_cast<int>(ctrl);
        return false;
    }
}

} // namespace component
} // namespace core
