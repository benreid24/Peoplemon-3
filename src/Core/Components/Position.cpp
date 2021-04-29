#include <Core/Components/Position.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
Position::Position()
: direction(Direction::Up)
, position(0, 0)
, level(0)
, interpolatedPosition(0.f, 0.f) {}

void Position::setTiles(const sf::Vector2i& tiles) {
    position               = tiles;
    interpolatedPosition.x = static_cast<float>(tiles.x * Properties::PixelsPerTile());
    interpolatedPosition.y = static_cast<float>(tiles.y * Properties::PixelsPerTile());
}

void Position::setPixels(const sf::Vector2f& interp) { interpolatedPosition = interp; }

const sf::Vector2i& Position::positionTiles() const { return position; }

const sf::Vector2f& Position::positionPixels() const { return interpolatedPosition; }

Position Position::move(Direction dir) const {
    Position n = *this;
    if (dir == direction) {
        switch (dir) {
        case Direction::Up:
            --n.position.y;
            break;
        case Direction::Right:
            ++n.position.x;
            break;
        case Direction::Down:
            ++n.position.y;
            break;
        case Direction::Left:
            --n.position.x;
            break;
        default:
            BL_LOG_WARN << "Unknown direction: " << static_cast<int>(dir);
            break;
        }
    }
    else {
        n.direction = dir;
    }
    return n;
}

} // namespace component
} // namespace core
