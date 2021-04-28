#include <Core/Components/Position.hpp>

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

} // namespace component
} // namespace core
