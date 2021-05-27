#include <Core/Systems/Cameras/Camera.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace camera
{
Camera::Camera()
: position(400.f, 300.f)
, size(1.f) {}

Camera::Camera(const sf::Vector2f& pos, float z)
: position(pos)
, size(z) {}

const sf::Vector2f& Camera::getPosition() const { return position; }

float Camera::getSize() const { return size; }

} // namespace camera
} // namespace system
} // namespace core
