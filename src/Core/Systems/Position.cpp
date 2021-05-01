#include <Core/Systems/Position.hpp>

#include <Core/Components/Collision.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>
#include <cmath>

namespace core
{
namespace system
{
Position::Position(Systems& owner)
: owner(owner) {}

void Position::init() {
    entities = owner.engine().entities().getEntitiesWithComponents<component::Position>();
    for (const auto& pair : *entities) {
        observe(bl::entity::event::ComponentAdded<component::Position>(
            pair.first, *pair.second.get<component::Position>()));
    }
}

void Position::update() {
    const sf::View area = owner.cameras().getView();
    const sf::Vector2f cornerf(area.getCenter() - area.getSize() * 2.f);
    const sf::Vector2f sizef(area.getSize() * 4.f);
    const sf::Vector2u corner(
        static_cast<unsigned int>(std::max(cornerf.x / Properties::PixelsPerTile(), 0.f)),
        static_cast<unsigned int>(std::max(cornerf.y / Properties::PixelsPerTile(), 0.f)));
    const sf::Vector2u size(
        std::min(static_cast<unsigned int>(std::ceil(sizef.x / Properties::PixelsPerTile())),
                 entityMap.front().getWidth() - corner.x - 1),
        std::min(static_cast<unsigned int>(std::ceil(sizef.y / Properties::PixelsPerTile())),
                 entityMap.front().getHeight() - corner.y - 1));

    toUpdate.clear();
    toUpdate.reserve(entityMap.size() * size.x * size.y / 4);
    for (const auto& vec2d : entityMap) {
        for (unsigned int x = corner.x; x < corner.x + size.x; ++x) {
            for (unsigned int y = corner.y; y < corner.y + size.y; ++y) {
                const bl::entity::Entity e = vec2d(x, y);
                if (e != bl::entity::InvalidEntity) toUpdate.emplace_back(e);
            }
        }
    }
}

const std::vector<bl::entity::Entity>& Position::updateRangeEntities() const { return toUpdate; }

bool Position::spaceFree(const component::Position& position) const {
    const bl::entity::Entity e =
        entityMap.at(position.level)(position.positionTiles().x, position.positionTiles().y);
    return e == bl::entity::InvalidEntity ||
           !owner.engine().entities().hasComponent<component::Collision>(e);
}

bl::entity::Entity Position::search(const component::Position& start, component::Direction dir,
                                    unsigned int range) {
    component::Position spos = start;
    spos.direction           = dir;
    for (unsigned int i = 0; i < range; ++i) {
        if (!owner.world().activeMap().movePossible(spos, dir)) break;
        spos                       = owner.world().activeMap().adjacentTile(spos, dir);
        const bl::entity::Entity e = get(spos);
        if (e != bl::entity::InvalidEntity) return e;
    }

    return bl::entity::InvalidEntity;
}

void Position::observe(const event::EntityMoved& event) {
    get(event.previousPosition) = bl::entity::InvalidEntity;
    get(event.position)         = event.entity;
}

void Position::observe(const bl::entity::event::ComponentAdded<component::Position>& event) {
    get(event.component) = event.entity;
}

void Position::observe(const bl::entity::event::EntityDestroyed& event) {
    auto it = entities->results().find(event.entity);
    if (it != entities->results().end()) {
        const component::Position& pos = *(it->second.get<component::Position>());
        get(pos)                       = bl::entity::InvalidEntity;
    }
}

void Position::observe(const bl::entity::event::ComponentRemoved<component::Position>& event) {
    get(event.component) = bl::entity::InvalidEntity;
}

void Position::observe(const event::MapSwitch& event) {
    entityMap.clear();
    entityMap.reserve(event.map.levelCount());
    for (std::uint8_t i = 0; i < event.map.levelCount(); ++i) {
        entityMap.emplace_back(
            event.map.sizeTiles().x, event.map.sizeTiles().y, bl::entity::InvalidEntity);
    }
}

bl::entity::Entity& Position::get(const component::Position& p) {
    static bl::entity::Entity null = bl::entity::InvalidEntity;

    if (p.level < entityMap.size()) {
        if (p.positionTiles().x >= 0 && p.positionTiles().x < entityMap[p.level].getWidth()) {
            if (p.positionTiles().y >= 0 && p.positionTiles().y < entityMap[p.level].getHeight()) {
                return entityMap[p.level](p.positionTiles().x, p.positionTiles().y);
            }
        }
    }

    BL_LOG_WARN << "Out of bounds entity check at (" << p.level << ", " << p.positionTiles().x
                << ", " << p.positionTiles().y << ")";
    null = bl::entity::InvalidEntity;
    return null;
}

bl::entity::Entity Position::getEntity(const component::Position& p) const {
    if (p.level < entityMap.size()) {
        if (p.positionTiles().x >= 0 && p.positionTiles().x < entityMap[p.level].getWidth()) {
            if (p.positionTiles().y >= 0 && p.positionTiles().y < entityMap[p.level].getHeight()) {
                return entityMap.at(p.level)(p.positionTiles().x, p.positionTiles().y);
            }
        }
    }

    BL_LOG_WARN << "Out of bounds entity check at (" << p.level << ", " << p.positionTiles().x
                << ", " << p.positionTiles().y << ")";
    return bl::entity::InvalidEntity;
}

} // namespace system
} // namespace core
