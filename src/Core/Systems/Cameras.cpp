#include <Core/Systems/Cameras.hpp>

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
void constrainView(const map::Map& map, sf::View& view) {
    const sf::Vector2f worldSize = map.sizePixels();

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
} // namespace

Cameras::Cameras(Systems& owner)
: owner(owner) {}

camera::Camera::Ptr Cameras::activeCamera() { return cameras.empty() ? nullptr : cameras.top(); }

void Cameras::pushCamera(camera::Camera::Ptr cam) { cameras.push(cam); }

void Cameras::replaceCamera(camera::Camera::Ptr cam) {
    if (!cameras.empty()) { cameras.top() = cam; }
    else {
        pushCamera(cam);
    }
}

void Cameras::popCamera() {
    if (cameras.size() > 1) cameras.pop();
}

void Cameras::clearAndReplace(camera::Camera::Ptr cam) {
    while (!cameras.empty()) cameras.pop();
    cameras.push(cam);
}

void Cameras::configureView(const map::Map& map, sf::View& view) const {
    if (!cameras.empty()) {
        view.setCenter(cameras.top()->getPosition());
        view.setSize(view.getSize() * cameras.top()->getSize());
        constrainView(map, view);
    }
}

sf::FloatRect Cameras::getArea() const {
    static const sf::Vector2f baseSize(static_cast<float>(Properties::WindowWidth()),
                                       static_cast<float>(Properties::WindowHeight()));
    sf::FloatRect area(0.f, 0.f, baseSize.x, baseSize.y);
    if (!cameras.empty()) {
        const sf::Vector2f pos  = cameras.top()->getPosition();
        const sf::Vector2f size = baseSize * cameras.top()->getSize();
        area.left               = pos.x - size.x * 0.5f;
        area.top                = pos.y - size.y * 0.5f;
        area.width              = size.x;
        area.height             = size.y;
    }

    return area;
}

void Cameras::update(float dt) {
    while (!cameras.top()->valid() && cameras.size() > 1) {
        cameras.pop();
        BL_LOG_INFO << "Popped invalidated camera";
    }

    if (!cameras.empty()) {
        if (!cameras.top()->valid()) { BL_LOG_WARN << "Only available camera is invalidated"; }
        cameras.top()->update(owner, dt);
    }
}

} // namespace system
} // namespace core
