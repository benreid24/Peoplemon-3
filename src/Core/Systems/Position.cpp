#include <Core/Systems/Position.hpp>

#include <Core/Components/Collision.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Position::Position(Systems& owner)
: owner(owner)
, spatialEntities(100.f, 100.f, 10.f, 10.f) {}

void Position::init() {
    entities = owner.engine().entities().getEntitiesWithComponents<component::Position>();

    for (const auto& pair : *entities) {
        observe(bl::entity::event::ComponentAdded<component::Position>(
            pair.first, *pair.second.get<component::Position>()));
    }
}

void Position::update() {
    const sf::View area = owner.cameras().getView();
    const sf::Vector2f topCorner(area.getCenter() - area.getSize() * 2.f);
    const sf::Vector2f size(area.getSize() * 4.f);
    inRange = spatialEntities.getArea(topCorner.x, topCorner.y, size.x, size.y);
}

const bl::container::Grid<bl::entity::Entity>::Range& Position::updateRangeEntities() const {
    return inRange;
}

bool Position::spaceFree(const component::Position& position) const {
    bl::container::Grid<bl::entity::Entity>& grid =
        const_cast<bl::container::Grid<bl::entity::Entity>&>(spatialEntities);
    auto area = grid.getCellAndNeighbors(position.positionPixels().x, position.positionPixels().y);
    for (const auto& ptr : area) {
        auto it = entities->results().find(ptr.get());
        if (it != entities->end()) {
            const component::Position& pos = *it->second.get<component::Position>();
            if (pos.positionTiles() == position.positionTiles() && pos.level == position.level &&
                owner.engine().entities().hasComponent<component::Collision>(it->first)) {
                return false;
            }
        }
    }
    return true;
}

bl::entity::Entity Position::search(const component::Position& start, component::Direction dir,
                                    unsigned int range) {
    component::Position spos = start;
    spos.direction           = dir;
    if (!owner.world().activeMap().movePossible(spos, dir)) return bl::entity::InvalidEntity;
    spos = owner.world().activeMap().adjacentTile(spos, dir);

    component::Position endPos = spos;
    for (unsigned int i = 1; i < range; ++i) { endPos = endPos.move(dir); }

    const float x = std::min(spos.positionPixels().x, endPos.positionPixels().x);
    const float y = std::min(spos.positionPixels().y, endPos.positionPixels().y);
    const float w =
        std::abs(spos.positionPixels().x - endPos.positionPixels().x) + Properties::PixelsPerTile();
    const float h =
        std::abs(spos.positionPixels().y - endPos.positionPixels().y) + Properties::PixelsPerTile();
    auto area = spatialEntities.getArea(x, y, w, h);
    if (area.empty()) return bl::entity::InvalidEntity;

    for (unsigned int i = 0; i < range; ++i) {
        for (const auto& ptr : area) {
            auto it = entities->results().find(ptr.get());
            if (it != entities->end()) {
                const component::Position& pos = *it->second.get<component::Position>();
                if (pos.positionTiles() == spos.positionTiles() && pos.level == spos.level) {
                    return it->first;
                }
            }
        }

        if (!owner.world().activeMap().movePossible(spos, dir)) break;
        spos = owner.world().activeMap().adjacentTile(spos, dir);
    }
    return bl::entity::InvalidEntity;
}

void Position::observe(const event::EntityMoved& event) {
    auto it = entityMap.find(event.entity);
    if (it != entityMap.end()) {
        it->second->move(event.position.positionPixels().x, event.position.positionPixels().y);
    }
    else {
        entityMap.emplace(event.entity,
                          spatialEntities.add(event.position.positionPixels().x,
                                              event.position.positionPixels().y,
                                              event.entity));
    }
}

void Position::observe(const bl::entity::event::ComponentAdded<component::Position>& event) {
    entityMap.emplace(event.entity,
                      spatialEntities.add(event.component.positionPixels().x,
                                          event.component.positionPixels().y,
                                          event.entity));
}

void Position::observe(const bl::entity::event::EntityDestroyed& event) {
    auto it = entityMap.find(event.entity);
    if (it != entityMap.end()) {
        it->second->remove();
        entityMap.erase(it);
    }
}

void Position::observe(const bl::entity::event::ComponentRemoved<component::Position>& event) {
    auto it = entityMap.find(event.entity);
    if (it != entityMap.end()) {
        it->second->remove();
        entityMap.erase(it);
    }
}

void Position::observe(const event::MapEntered& event) {
    const float cellSize = static_cast<float>(Properties::PixelsPerTile() * 5);
    spatialEntities.setSize(event.map.sizePixels().x, event.map.sizePixels().y, cellSize, cellSize);
    entityMap.clear();
    init();
}

} // namespace system
} // namespace core
