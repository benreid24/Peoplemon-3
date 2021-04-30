#include <Core/Systems/Movement.hpp>

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
        if (!pos.moving()) {
            component::Position npos = owner.world().activeMap().adjacentTile(pos, dir);
            if (npos.positionTiles() == pos.positionTiles()) {
                pos = npos; // TODO - send event on rotate?
                return true;
            }
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
        if (it != entities->results().end()) { it->second.get<component::Movable>()->update(dt); }
    }
}

} // namespace system
} // namespace core