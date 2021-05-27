#include <Core/Systems/Cameras/Follow.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace camera
{
Camera::Ptr Follow::create(Systems& systems, bl::entity::Entity e) {
    return Camera::Ptr(new Follow(systems, e));
}

Follow::Follow(Systems& systems, bl::entity::Entity e)
: Camera()
, owner(e)
, ownerPos(systems.engine().entities().getComponentHandle<component::Position>(e))
, alive(true) {
    if (!ownerPos.hasValue()) {
        BL_LOG_ERROR << "Follow camera created with invalid entity: " << e;
        alive = false;
    }
    else {
        position = ownerPos.get().positionPixels();
    }
}

bool Follow::valid() const { return alive; }

void Follow::update(Systems&, float) {
    if (valid()) { position = ownerPos.get().positionPixels(); }
}

void Follow::observe(const bl::entity::event::EntityDestroyed& event) {
    if (event.entity == owner) alive = false;
}

} // namespace camera
} // namespace system
} // namespace core
