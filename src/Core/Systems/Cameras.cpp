#include <Core/Systems/Cameras.hpp>

#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
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

const sf::View& Cameras::getView() const {
    static const sf::View defaultView({0.f,
                                       0.f,
                                       static_cast<float>(Properties::WindowWidth()),
                                       static_cast<float>(Properties::WindowHeight())});
    return cameras.empty() ? defaultView : cameras.top()->getView();
}

void Cameras::update(float dt) {
    while (!cameras.top()->valid() && cameras.size() > 1) {
        cameras.pop();
        BL_LOG_INFO << "Popped invalidated camera";
    }
    if (!cameras.top()->valid()) { BL_LOG_WARN << "Only available camera is invalidated"; }

    if (!cameras.empty()) { cameras.top()->update(owner, dt); }
}

} // namespace system
} // namespace core
