#include <Core/Components/Direction.hpp>

namespace core
{
namespace component
{
Direction oppositeDirection(Direction dir) {
    const unsigned int i = static_cast<unsigned int>(dir);
    return static_cast<Direction>((i + 2) % 4);
}

Direction nextClockwiseDirection(Direction dir) {
    const unsigned int i = static_cast<unsigned int>(dir);
    return static_cast<Direction>((i + 1) % 4);
}

Direction nextCounterClockwiseDirection(Direction dir) {
    switch (dir) {
    case Direction::Up:
        return Direction::Left;
    case Direction::Right:
        return Direction::Up;
    case Direction::Down:
        return Direction::Right;
    case Direction::Left:
        return Direction::Down;
    default:
        return Direction::Up;
    }
}

} // namespace component
} // namespace core
