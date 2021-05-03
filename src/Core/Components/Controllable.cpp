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

bool Controllable::processControl(Control ctrl) {
    switch (ctrl) {
    case Control::MoveUp:
        return systems.movement().moveEntity(owner, Direction::Up, false);
    case Control::MoveRight:
        return systems.movement().moveEntity(owner, Direction::Right, false);
    case Control::MoveDown:
        return systems.movement().moveEntity(owner, Direction::Down, false);
    case Control::MoveLeft:
        return systems.movement().moveEntity(owner, Direction::Left, false);

    case Control::SprintUp:
        return systems.movement().moveEntity(owner, Direction::Up, true);
    case Control::SprintRight:
        return systems.movement().moveEntity(owner, Direction::Right, true);
    case Control::SprintDown:
        return systems.movement().moveEntity(owner, Direction::Down, true);
    case Control::SprintLeft:
        return systems.movement().moveEntity(owner, Direction::Left, true);

    case Control::Pause:
        // TODO - pause menu
        return false;

    case Control::Back:
        // TODO - back menu?
        return false;

    case Control::Interact:
        // TODO - interaction system
        return false;

    default:
        BL_LOG_WARN << "Unknown control: " << static_cast<int>(ctrl);
        return false;
    }
}

} // namespace component
} // namespace core
