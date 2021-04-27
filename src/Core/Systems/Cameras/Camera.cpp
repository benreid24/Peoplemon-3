#include <Core/Systems/Cameras/Camera.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace camera
{
const sf::View& Camera::getView() const { return view; }

void Camera::constrainView(Systems& owner) {
    // TODO
}

} // namespace camera
} // namespace system
} // namespace core
