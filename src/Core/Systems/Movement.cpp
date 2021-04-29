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

bool Movement::moveEntity(bl::entity::Entity e, component::Direction dir, bool fast) {
    auto it = entities->results().find(e);
    if (it != entities->results().end()) {
        component::Position& pos = *(it->second.get<component::Position>());
        if (!pos.moving()) {
            const component::Position npos = owner.world().activeMap().adjacentTile(pos, dir);
            if (npos.positionTiles() == pos.positionTiles()) {
                pos = npos;
                return true;
            }
            if (!owner.position().spaceFree(npos)) return false;
            if (!owner.world().activeMap().movePossible(pos, dir)) return false;

            pos                                              = npos;
            it->second.get<component::Movable>()->movingFast = fast;
            return true;
        }
    }
    return false;
}

void Movement::update(float dt) {
    for (auto& payload : owner.position().updateRangeEntities()) {
        auto it = entities->results().find(payload.get());
        if (it != entities->results().end()) {
            component::Position& pos = *(it->second.get<component::Position>());
            if (pos.moving()) {
                const component::Movable& move = *(it->second.get<component::Movable>());
                const float speed = move.movingFast ? move.fastMovementSpeed : move.movementSpeed;
                const float displacement = dt * speed;

                const float xTile = pos.positionTiles().x * Properties::PixelsPerTile();
                const float yTile = pos.positionTiles().y * Properties::PixelsPerTile();
                if (pos.positionPixels().x > xTile) {
                    pos.setPixels(pos.positionPixels() + sf::Vector2f(displacement, 0.f));
                    if (pos.positionPixels().x >= xTile) { pos.setTiles(pos.positionTiles()); }
                }
                else if (pos.positionPixels().x > xTile) {
                    pos.setPixels(pos.positionPixels() - sf::Vector2f(displacement, 0.f));
                    if (pos.positionPixels().x <= xTile) { pos.setTiles(pos.positionTiles()); }
                }
                if (pos.positionPixels().y < yTile) {
                    pos.setPixels(pos.positionPixels() + sf::Vector2f(0.f, displacement));
                    if (pos.positionPixels().x >= yTile) { pos.setTiles(pos.positionTiles()); }
                }
                else if (pos.positionPixels().y > yTile) {
                    pos.setPixels(pos.positionPixels() - sf::Vector2f(0.f, -displacement));
                    if (pos.positionPixels().x <= yTile) { pos.setTiles(pos.positionTiles()); }
                }
            }
        }
    }
}

} // namespace system
} // namespace core