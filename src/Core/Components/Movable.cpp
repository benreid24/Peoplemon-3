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

glm::vec2 getPixels(bl::tmap::Direction dir, float x, float y, float interp) {
    switch (dir) {
    case bl::tmap::Direction::Up:
        return {x, y + interp};
    case bl::tmap::Direction::Right:
        return {x - interp, y};
    case bl::tmap::Direction::Down:
        return {x, y - interp};
    case bl::tmap::Direction::Left:
    default:
        return {x + interp, y};
    }
}
} // namespace

Movable::Movable(bl::tmap::Position& pos, float speed, float fastSpeed)
: position(pos)
, movementSpeed(speed)
, fastMovementSpeed(fastSpeed)
, state(MoveState::Still) {}

bool Movable::moving() const { return state != MoveState::Still; }

bool Movable::goingFast() const { return state == MoveState::MovingFast; }

void Movable::move(bl::tmap::Direction dir, bool fast, bool isHop) {
    moveDir = dir;
    state   = fast ? MoveState::MovingFast : (isHop ? MoveState::LedgeHopping : MoveState::Moving);
    interpRemaining = static_cast<float>(Properties::PixelsPerTile()) * (isHop ? 2.f : 1.f);
}

void Movable::update(bl::ecs::Entity owner, bl::engine::Engine& engine, float dt) {
    if (state != MoveState::Still) {
        if (!position.transform) {
            BL_LOG_WARN << "Entity with movable component missing transform: " << owner;
            return;
        }

        if (state != MoveState::LedgeHopping) {
            // interpolate
            const float speed = state == MoveState::MovingFast ? fastMovementSpeed : movementSpeed;
            const float displacement = dt * speed;
            interpRemaining          = std::max(interpRemaining - displacement, 0.f);

            // set render pos
            const float xTile = position.position.x * Properties::PixelsPerTile();
            const float yTile = position.position.y * Properties::PixelsPerTile();
            position.transform->setPosition(getPixels(moveDir, xTile, yTile, interpRemaining));

            if (interpRemaining <= 0.f) {
                position.syncTransform(Properties::PixelsPerTile());
                state = MoveState::Still;
                bl::event::Dispatcher::dispatch<event::EntityMoveFinished>({owner, position});
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
            Renderable* anim = engine.ecs().getComponent<Renderable>(owner);
            if (anim) { anim->updateShadow(height, ShadowSize - ShadowShrinkage * hp); }

            // set render pos
            const float xTile = position.position.x * Properties::PixelsPerTile();
            const float yTile = position.position.y * Properties::PixelsPerTile();
            position.transform->setPosition(xTile, yTile - interpRemaining - height);

            // check finished
            if (interpRemaining <= 0.f) {
                position.syncTransform(Properties::PixelsPerTile());
                state = MoveState::Still;
                bl::event::Dispatcher::dispatch<event::EntityMoveFinished>({owner, position});
                if (anim) { anim->removeShadow(); }
            }
        }
    }
}

} // namespace component
} // namespace core
