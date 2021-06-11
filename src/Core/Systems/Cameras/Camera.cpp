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

void Camera::configureView(sf::View& view, const sf::Vector2f& area) {
    view.setCenter(position);
    view.setSize(view.getSize() * size);

    if (view.getSize().x > area.x) { view.setCenter(area.x / 2.f, view.getCenter().y); }
    else if (view.getCenter().x - view.getSize().x / 2.f < 0.f) {
        view.setCenter(view.getSize().x / 2.f, view.getCenter().y);
        position = view.getCenter();
    }
    else if (view.getCenter().x + view.getSize().x / 2.f >= area.x) {
        view.setCenter(area.x - view.getSize().x / 2.f, view.getCenter().y);
        position = view.getCenter();
    }

    if (view.getSize().y > area.y) { view.setCenter(view.getCenter().x, area.y / 2.f); }
    else if (view.getCenter().y - view.getSize().y / 2.f < 0.f) {
        view.setCenter(view.getCenter().x, view.getSize().y / 2.f);
        position = view.getCenter();
    }
    else if (view.getCenter().y + view.getSize().y / 2.f >= area.y) {
        view.setCenter(view.getCenter().x, area.y - view.getSize().y / 2.f);
        position = view.getCenter();
    }
}

} // namespace camera
} // namespace system
} // namespace core
