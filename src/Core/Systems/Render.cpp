#include <Core/Systems/Render.hpp>

#include <Core/Properties.hpp>
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

    for (const bl::ecs::Entity shadow : shadowsToRemove) {
        owner.engine().ecs().destroyEntity(shadow);
    }
    shadowsToRemove.clear();
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

void Render::updateShadow(bl::ecs::Entity entity, float distance, float radius) {
    component::Renderable* rc = pool.get(entity);
    if (!rc) {
        BL_LOG_WARN << "Cannot update shadow of entity, missing Renderable component: " << entity;
        return;
    }

    constexpr float Radius = 100.f;

    bl::gfx::Circle* shadow = nullptr;
    if (rc->shadow != bl::ecs::InvalidEntity) {
        shadow = owner.engine().ecs().getComponent<bl::gfx::Circle>(rc->shadow);
        if (!shadow) {
            BL_LOG_ERROR << "Entity is missing shadow but one was expected, re-creating";
            owner.engine().ecs().destroyEntity(rc->shadow);
        }
    }
    if (!shadow) {
        rc->shadow = owner.engine().ecs().createEntity(bl::ecs::Flags::WorldObject);
        owner.engine().ecs().setEntityParent(rc->shadow, entity);
        shadow = owner.engine().ecs().emplaceComponent<bl::gfx::Circle>(rc->shadow);
        shadow->create(owner.engine(), rc->shadow, Radius);
        shadow->deleteEntityOnDestroy(false);
        shadow->getTransform().setOrigin(Radius, Radius);
        shadow->getTransform().setDepth(0.5f); // just below parent
        shadow->setFillColor(sf::Color(0, 0, 0, 145));
        shadow->addToScene(owner.world().activeMap().getScene(), bl::rc::UpdateSpeed::Dynamic);
    }

    bl::com::Transform2D* parentTransform =
        owner.engine().ecs().getComponent<bl::com::Transform2D>(entity);
    const glm::vec2 parentOrigin =
        parentTransform ? parentTransform->getOrigin() : glm::vec2{16.f, 32.f};

    shadow->scaleToSize({radius * 2.f, radius * 2.f});
    shadow->getTransform().setPosition(parentOrigin.x + Properties::PixelsPerTile() / 2,
                                       parentOrigin.y + distance + Properties::PixelsPerTile() / 2);
}

void Render::removeShadow(bl::ecs::Entity entity) {
    component::Renderable* rc = pool.get(entity);
    if (rc && rc->shadow != bl::ecs::InvalidEntity) {
        shadowsToRemove.emplace_back(rc->shadow);
        rc->shadow = bl::ecs::InvalidEntity;
    }
}

} // namespace system
} // namespace core
