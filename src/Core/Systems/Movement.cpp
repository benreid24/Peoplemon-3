#include <Core/Systems/Movement.hpp>

#include <Core/Events/EntityMoved.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Movement::Movement(Systems& owner)
: owner(owner) {
    jumpSound = bl::audio::AudioSystem::getOrLoadSound(
        bl::util::FileUtil::joinPath(Properties::SoundPath(), "World/jump.wav"));
}

bool Movement::makeMovable(bl::ecs::Entity e, float sp, float fsp) {
    if (owner.engine().ecs().hasComponent<component::Movable>(e)) return false;
    bl::tmap::Position* pos = owner.engine().ecs().getComponent<bl::tmap::Position>(e);
    if (!pos) return false;
    owner.engine().ecs().addComponent<component::Movable>(e, {*pos, sp, fsp});
    return true;
}

bool Movement::moveEntity(bl::ecs::Entity e, bl::tmap::Direction dir, bool fast) {
    component::Movable* mv = owner.engine().ecs().getComponent<component::Movable>(e);
    if (mv != nullptr) {
        bl::tmap::Position& pos = mv->position;
        if (!mv->moving()) {
            bl::tmap::Position npos = owner.world().activeMap().adjacentTile(pos, dir);
            if (npos.position == pos.position) {
                const event::EntityRotated event(e, npos.direction, pos.direction);
                pos.direction = npos.direction;
                bl::event::Dispatcher::dispatch<event::EntityRotated>(event);
                return true;
            }
            if (!owner.world().activeMap().contains(npos)) return false;
            if (!owner.position().spaceFree(npos)) return false;
            if (!owner.world().activeMap().movePossible(pos, dir)) return false;

            const bool isHop = owner.world().activeMap().isLedgeHop(pos, dir);
            if (isHop) {
                bl::audio::AudioSystem::playOrRestartSound(jumpSound);
                npos.position.y += 1;
                npos.syncTransform(Properties::PixelsPerTile());
            }

            std::swap(pos, npos);
            mv->move(dir, fast, isHop);
            bl::event::Dispatcher::dispatch<event::EntityMoved>({e, npos, pos});
            return true;
        }
    }
    return false;
}

void Movement::update(float dt) {
    bl::engine::Engine& engine = owner.engine();

    engine.ecs().getAllComponents<component::Movable>().forEach(
        [dt, &engine](bl::ecs::Entity ent, component::Movable& mv) { mv.update(ent, engine, dt); });
}

} // namespace system
} // namespace core
