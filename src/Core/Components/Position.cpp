#include <Core/Components/Position.hpp>

namespace core
{
namespace entity
{
Position::Position()
: dir(*this, Direction::Up)
, pos(*this, {0, 0})
, lev(*this, 0)
, direction(dir.getValue())
, position(pos.getValue())
, interpolatedPosition(0, 0)
, level(lev.getValue()) {}

Position::Position(const Position& copy)
: Position() {
    *this = copy;
}

Position& Position::operator=(const Position& copy) {
    direction            = copy.direction;
    position             = copy.position;
    interpolatedPosition = copy.interpolatedPosition;
    level                = copy.level;
    return *this;
}

} // namespace entity
} // namespace core
