#include <Core/Cameras/ShakeFollow.hpp>

#include <BLIB/Math.hpp>
#include <Core/Cameras/Util.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace camera
{
const sf::Vector2f* getPositionPointer(system::Systems& s, bl::entity::Entity e) {
    component::Position* pos = s.engine().entities().getComponent<component::Position>(e);
    return pos ? &pos->positionPixels() : nullptr;
}

ShakeFollow::Ptr ShakeFollow::create(system::Systems& s, bl::entity::Entity e, float sps) {
    return Ptr{new ShakeFollow(s, e, sps)};
}

ShakeFollow::ShakeFollow(system::Systems& s, bl::entity::Entity e, float sps)
: FollowCamera(getPositionPointer(s, e), Properties::WindowSize())
, shakesPerSec(sps)
, magnitude(0.f)
, time(0.f) {}

void ShakeFollow::update(float dt) {
    FollowCamera::update(dt);

    time += dt;
    const float t = shakesPerSec * time * 360.f;
    const sf::Vector2f noise(bl::math::cos(-t), -bl::math::cos(t));
    setCenter(getCenter() + noise * magnitude);
}

void ShakeFollow::setShakesPerSecond(float sps) { shakesPerSec = sps; }

void ShakeFollow::setMagnitude(float m) { magnitude = m; }

} // namespace camera
} // namespace core
