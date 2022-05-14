#include <Core/Systems/Movement.hpp>

#include <Core/Events/EntityMoved.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Movement::Movement(Systems& owner)
: owner(owner) {}

void Movement::init() {
    entities = owner.engine()
                   .entities()
                   .getEntitiesWithComponents<component::Position, component::Movable>();
}

bool Movement::makeMovable(bl::entity::Entity e, float sp, float fsp) {
    if (owner.engine().entities().hasComponent<component::Movable>(e)) return false;
    bl::entity::Registry::ComponentHandle<component::Position> h =
        owner.engine().entities().getComponentHandle<component::Position>(e);
    if (!h.hasValue()) return false;
    owner.engine().entities().addComponent<component::Movable>(e, {h, sp, fsp});
    return true;
}

bool Movement::moveEntity(bl::entity::Entity e, component::Direction dir, bool fast) {
    auto it = entities->results().find(e);
    if (it != entities->results().end()) {
        component::Position& pos = *(it->second.get<component::Position>());
        if (!it->second.get<component::Movable>()->moving()) {
            component::Position npos = owner.world().activeMap().adjacentTile(pos, dir);
            if (npos.positionTiles() == pos.positionTiles()) {
                const event::EntityRotated event(e, npos.direction, pos.direction);
                pos.direction = npos.direction;
                owner.engine().eventBus().dispatch<event::EntityRotated>(event);
                return true;
            }
            if (!owner.world().activeMap().contains(npos)) return false;
            if (!owner.position().spaceFree(npos)) return false;
            if (!owner.world().activeMap().movePossible(pos, dir)) return false;

            std::swap(pos, npos);
            it->second.get<component::Movable>()->move(dir, fast);
            owner.engine().eventBus().dispatch<event::EntityMoved>({e, npos, pos});
            return true;
        }
    }
    return false;
}

void Movement::update(float dt) {
    for (const bl::entity::Entity e : owner.position().updateRangeEntities()) {
        auto it = entities->results().find(e);
        if (it != entities->results().end()) {
            it->second.get<component::Movable>()->update(e, owner.engine().eventBus(), dt);
        }
    }
}

} // namespace system
} // namespace core
