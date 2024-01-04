#include <Core/Systems/Render.hpp>

#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Render::Render(Systems& o)
: owner(o)
, pool(owner.engine().ecs().getAllComponents<component::Renderable>())
, transformPool(owner.engine().ecs().getAllComponents<bl::com::Transform2D>()) {}

void Render::update(std::mutex&, float, float, float, float) {
    for (const auto& epair : stopRm) {
        component::Renderable* rc = pool.get(epair.first);
        if (rc) { rc->notifyMoveState(epair.second, false, false); }
    }
    stopRm.clear();
}

void Render::init(bl::engine::Engine&) { bl::event::Dispatcher::subscribe(this); }

void Render::notifyFrameStart() { std::swap(stopAdd, stopRm); }

void Render::observe(const event::EntityMoved& event) {
    component::Renderable* rc = pool.get(event.entity);
    if (rc) {
        rc->notifyMoveState(event.position.direction, true, event.running);
        std::erase_if(stopRm, [&event](auto& pair) { return pair.first == event.entity; });
        std::erase_if(stopAdd, [&event](auto& pair) { return pair.first == event.entity; });
    }

    bl::com::Transform2D* transform = transformPool.get(event.entity);
    if (transform) {
        transform->setDepth(owner.world().activeMap().getDepthForPosition(
            event.position.level, event.position.position.y));
    }
}

void Render::observe(const event::EntityMoveFinished& event) {
    stopAdd.emplace_back(event.entity, event.position.direction);
}

void Render::observe(const event::EntityRotated& event) {
    component::Renderable* rc = pool.get(event.entity);
    if (rc) { rc->notifyMoveState(event.faceDir, false, false); }
}

} // namespace system
} // namespace core
