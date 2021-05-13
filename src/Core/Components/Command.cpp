#include <Core/Components/Command.hpp>

namespace core
{
namespace component
{
Command moveCommand(Direction dir, bool run) {
    switch (dir) {
    case Direction::Up:
        return run ? Command::SprintUp : Command::MoveUp;
    case Direction::Right:
        return run ? Command::SprintRight : Command::MoveRight;
    case Direction::Down:
        return run ? Command::SprintDown : Command::MoveDown;
    case Direction::Left:
        return run ? Command::SprintLeft : Command::MoveLeft;
    default:
        return Command::None;
    }
}

} // namespace component
} // namespace core
