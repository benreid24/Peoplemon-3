#include <Core/Components/Movable.hpp>

#include <Core/Events/EntityMoved.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
namespace
{
sf::Vector2f getPixels(Direction dir, float x, float y, float interp) {
    switch (dir) {
    case Direction::Up:
        return {x, y + interp};
    case Direction::Right:
        return {x - interp, y};
    case Direction::Down:
        return {x, y - interp};
    case Direction::Left:
    default:
        return {x + interp, y};
    }
}
} // namespace

Movable::Movable(const bl::entity::Registry::ComponentHandle<component::Position>& pos, float speed,
                 float fastSpeed)
: position(pos)
, movementSpeed(speed)
, fastMovementSpeed(fastSpeed)
, isMoving(false)
, movingFast(false) {}

bool Movable::moving() const { return isMoving; }

bool Movable::goingFast() const { return movingFast; }

void Movable::move(Direction dir, bool fast) {
    moveDir         = dir;
    isMoving        = true;
    movingFast      = fast;
    interpRemaining = static_cast<float>(Properties::PixelsPerTile());
}

void Movable::update(bl::entity::Entity owner, bl::event::Dispatcher& bus, float dt) {
    if (isMoving) {
        // interpolate
        const float speed        = movingFast ? fastMovementSpeed : movementSpeed;
        const float displacement = dt * speed;
        interpRemaining          = std::max(interpRemaining - displacement, 0.f);

        // set render pos
        const float xTile = position.get().positionTiles().x * Properties::PixelsPerTile();
        const float yTile = position.get().positionTiles().y * Properties::PixelsPerTile();
        position.get().setPixels(getPixels(moveDir, xTile, yTile, interpRemaining));

        if (interpRemaining <= 0.f) {
            position.get().setTiles(position.get().positionTiles());
            isMoving = false;
            bus.dispatch<event::EntityMoveFinished>({owner, position.get()});
        }
    }
}

} // namespace component
} // namespace core
