#include <Core/Components/StandingBehavior.hpp>

namespace core
{
namespace component
{
StandingBehavior::StandingBehavior(
    Direction faceDir, const bl::entity::Registry::ComponentHandle<Position>& position,
    const bl::entity::Registry::ComponentHandle<Controllable>& controllable)
: dir(faceDir)
, position(position)
, controllable(controllable) {}

void StandingBehavior::update() {
    if (position.get().direction != dir) {
        switch (dir) {
        case Direction::Down:
            controllable.get().processControl(Command::MoveDown);
            break;

        case Direction::Right:
            controllable.get().processControl(Command::MoveRight);
            break;

        case Direction::Up:
            controllable.get().processControl(Command::MoveUp);
            break;

        case Direction::Left:
            controllable.get().processControl(Command::MoveLeft);
            break;

        default:
            break;
        }
    }
}

} // namespace component
} // namespace core
