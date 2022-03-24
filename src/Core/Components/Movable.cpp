#include <Core/Components/Movable.hpp>

#include <Core/Events/EntityMoved.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
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
    moveDir    = dir;
    isMoving   = true;
    movingFast = fast;
}

void Movable::update(bl::entity::Entity owner, bl::event::Dispatcher& bus, float dt) {
    if (isMoving) {
        const float speed        = movingFast ? fastMovementSpeed : movementSpeed;
        const float displacement = dt * speed;

        const float xTile = position.get().positionTiles().x * Properties::PixelsPerTile();
        const float yTile = position.get().positionTiles().y * Properties::PixelsPerTile();

        switch (moveDir) {
        case Direction::Up:
            position.get().setPixels(position.get().positionPixels() -
                                     sf::Vector2f(0.f, displacement));
            if (position.get().positionPixels().y <= yTile) {
                position.get().setTiles(position.get().positionTiles());
                isMoving = false;
                bus.dispatch<event::EntityMoveFinished>({owner, position.get()});
            }
            break;

        case Direction::Right:
            position.get().setPixels(position.get().positionPixels() +
                                     sf::Vector2f(displacement, 0.f));
            if (position.get().positionPixels().x >= xTile) {
                position.get().setTiles(position.get().positionTiles());
                isMoving = false;
                bus.dispatch<event::EntityMoveFinished>({owner, position.get()});
            }
            break;

        case Direction::Down:
            position.get().setPixels(position.get().positionPixels() +
                                     sf::Vector2f(0.f, displacement));
            if (position.get().positionPixels().y >= yTile) {
                position.get().setTiles(position.get().positionTiles());
                isMoving = false;
                bus.dispatch<event::EntityMoveFinished>({owner, position.get()});
            }
            break;

        case Direction::Left:
            position.get().setPixels(position.get().positionPixels() -
                                     sf::Vector2f(displacement, 0.f));
            if (position.get().positionPixels().x <= xTile) {
                position.get().setTiles(position.get().positionTiles());
                isMoving = false;
                bus.dispatch<event::EntityMoveFinished>({owner, position.get()});
            }
            break;

        default:
            BL_LOG_WARN << "Invalid move dir: " << static_cast<int>(moveDir);
            position.get().setTiles(position.get().positionTiles());
            isMoving = false;
            break;
        }
    }
}

} // namespace component
} // namespace core
