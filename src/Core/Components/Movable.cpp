#include <Core/Components/Movable.hpp>

#include <Core/Components/Renderable.hpp>
#include <Core/Events/EntityMoved.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace component
{
namespace
{
constexpr float HopDistance       = 64.f;
constexpr float HopHeight         = 20.f;
constexpr float MaxHeightPoint    = 0.25f;
constexpr float HopUpHeightMult   = 1.f / MaxHeightPoint;
constexpr float HopDownHeightMult = 1.f / (1.f - MaxHeightPoint);
constexpr float ShadowSize        = 16.f;
constexpr float ShadowShrinkage   = 3.f;

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

Movable::Movable(const bl::entity::Registry::ComponentHandle<Position>& pos, float speed,
                 float fastSpeed)
: position(pos)
, movementSpeed(speed)
, fastMovementSpeed(fastSpeed)
, state(MoveState::Still) {}

bool Movable::moving() const { return state != MoveState::Still; }

bool Movable::goingFast() const { return state == MoveState::MovingFast; }

void Movable::move(Direction dir, bool fast, bool isHop) {
    moveDir = dir;
    state   = fast ? MoveState::MovingFast : (isHop ? MoveState::LedgeHopping : MoveState::Moving);
    interpRemaining = static_cast<float>(Properties::PixelsPerTile()) * (isHop ? 2.f : 1.f);
}

void Movable::update(bl::entity::Entity owner, bl::engine::Engine& engine, float dt) {
    if (state != MoveState::Still) {
        if (state != MoveState::LedgeHopping) {
            // interpolate
            const float speed = state == MoveState::MovingFast ? fastMovementSpeed : movementSpeed;
            const float displacement = dt * speed;
            interpRemaining          = std::max(interpRemaining - displacement, 0.f);

            // set render pos
            const float xTile = position.get().positionTiles().x * Properties::PixelsPerTile();
            const float yTile = position.get().positionTiles().y * Properties::PixelsPerTile();
            position.get().setPixels(getPixels(moveDir, xTile, yTile, interpRemaining));

            if (interpRemaining <= 0.f) {
                position.get().setTiles(position.get().positionTiles());
                state = MoveState::Still;
                engine.eventBus().dispatch<event::EntityMoveFinished>({owner, position.get()});
            }
        }
        else {
            interpRemaining     = std::max(interpRemaining - movementSpeed * dt, 0.f);
            const float percent = 1.f - interpRemaining / HopDistance;
            const float hp      = percent <= MaxHeightPoint ?
                                      percent * HopUpHeightMult :
                                      1.f - (percent - MaxHeightPoint) * HopDownHeightMult;
            const float height  = HopHeight * hp;

            // update shadow
            Renderable* anim = engine.entities().getComponent<Renderable>(owner);
            if (anim) { anim->updateShadow(height, ShadowSize - ShadowShrinkage * hp); }

            // set render pos
            const float xTile = position.get().positionTiles().x * Properties::PixelsPerTile();
            const float yTile = position.get().positionTiles().y * Properties::PixelsPerTile();
            position.get().setPixels({xTile, yTile - interpRemaining - height});

            // check finished
            if (interpRemaining <= 0.f) {
                position.get().setTiles(position.get().positionTiles());
                state = MoveState::Still;
                engine.eventBus().dispatch<event::EntityMoveFinished>({owner, position.get()});
                if (anim) { anim->removeShadow(); }
            }
        }
    }
}

} // namespace component
} // namespace core
