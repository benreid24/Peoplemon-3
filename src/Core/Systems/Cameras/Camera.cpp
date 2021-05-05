#include <Core/Systems/Cameras/Camera.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace camera
{
Camera::Camera(const sf::View& view)
: view(view) {}

const sf::View& Camera::getView() const { return view; }

void Camera::constrainView(Systems& owner) {
    const sf::Vector2f worldSize = owner.world().activeMap().sizePixels();

    if (view.getSize().x > worldSize.x) { view.setCenter(worldSize.x / 2.f, view.getCenter().y); }
    else if (view.getCenter().x - view.getSize().x / 2.f < 0.f) {
        view.setCenter(view.getSize().x / 2.f, view.getCenter().y);
    }
    else if (view.getCenter().x + view.getSize().x / 2.f >= worldSize.x) {
        view.setCenter(worldSize.x - view.getSize().x / 2.f, view.getCenter().y);
    }

    if (view.getSize().y > worldSize.y) { view.setCenter(view.getCenter().x, worldSize.y / 2.f); }
    else if (view.getCenter().y - view.getSize().y / 2.f < 0.f) {
        view.setCenter(view.getCenter().x, view.getSize().y / 2.f);
    }
    else if (view.getCenter().y + view.getSize().y / 2.f >= worldSize.y) {
        view.setCenter(view.getCenter().x, worldSize.y - view.getSize().y / 2.f);
    }
}

} // namespace camera
} // namespace system
} // namespace core
