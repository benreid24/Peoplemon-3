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

void Movement::init() {
    entities = owner.engine().ecs().getOrCreateView<component::Position, component::Movable>();
}

bool Movement::makeMovable(bl::ecs::Entity e, float sp, float fsp) {
    if (owner.engine().ecs().hasComponent<component::Movable>(e)) return false;
    component::Position* pos = owner.engine().ecs().getComponent<component::Position>(e);
    if (!pos) return false;
    owner.engine().ecs().addComponent<component::Movable>(e, {*pos, sp, fsp});
    return true;
}

bool Movement::moveEntity(bl::ecs::Entity e, component::Direction dir, bool fast) {
    component::Movable* mv = owner.engine().ecs().getComponent<component::Movable>(e);
    if (mv != nullptr) {
        component::Position& pos = mv->position;
        if (!mv->moving()) {
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

            const bool isHop = owner.world().activeMap().isLedgeHop(pos, dir);
            if (isHop) {
                bl::audio::AudioSystem::playOrRestartSound(jumpSound);
                npos.setTiles(npos.positionTiles() + sf::Vector2i(0, 1));
            }

            std::swap(pos, npos);
            mv->move(dir, fast, isHop);
            owner.engine().eventBus().dispatch<event::EntityMoved>({e, npos, pos});
            return true;
        }
    }
    return false;
}

void Movement::update(float dt) {
    bl::engine::Engine& engine = owner.engine();

    const auto visitor = [&engine,
                          dt](bl::ecs::ComponentSet<component::Position, component::Movable>& cs) {
        cs.get<component::Movable>()->update(cs.entity(), engine, dt);
    };

    entities->forEach(visitor);
}

} // namespace system
} // namespace core
