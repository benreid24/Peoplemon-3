#include <Core/Systems/Cameras/ShakeFollow.hpp>

#include <BLIB/Math.hpp>

namespace core
{
namespace system
{
namespace camera
{
ShakeFollow::Ptr ShakeFollow::create(Systems& s, bl::entity::Entity e, float sps) {
    return Ptr{new ShakeFollow(s, e, sps)};
}

ShakeFollow::ShakeFollow(Systems& s, bl::entity::Entity e, float sps)
: Follow(s, e)
, shakesPerSec(sps)
, magnitude(0.f)
, time(0.f) {}

void ShakeFollow::update(Systems& s, float dt) {
    Follow::update(s, dt);

    time += dt;
    const float t = shakesPerSec * time * 360.f;
    const sf::Vector2f noise(bl::math::cos(-t), -bl::math::cos(t));
    position += noise * magnitude;
}

void ShakeFollow::setShakesPerSecond(float sps) { shakesPerSec = sps; }

void ShakeFollow::setMagnitude(float m) { magnitude = m; }

} // namespace camera
} // namespace system
} // namespace core
