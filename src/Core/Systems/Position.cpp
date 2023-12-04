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
    bl::event::Dispatcher::subscribe(this);
    owner.engine().ecs().getAllComponents<component::Position>().forEach(
        [this](bl::ecs::Entity ent, component::Position& pos) {
            observe(bl::ecs::event::ComponentAdded<component::Position>(ent, pos));
        });
}

void Position::update() {
    // TODO - BLIB_UPGRADE - refactor position component + system
    /*sf::FloatRect area = owner.engine().renderSystem().cameras().getCurrentViewport();
    area.left -= area.width * 1.5f;
    area.top -= area.height * 1.5f;
    area.width *= 3.f;
    area.height *= 3.f;
    const sf::Vector2u corner(
        static_cast<unsigned int>(std::max(area.left / Properties::PixelsPerTile(), 0.f)),
        static_cast<unsigned int>(std::max(area.top / Properties::PixelsPerTile(), 0.f)));
    const sf::Vector2u size(
        std::min(static_cast<unsigned int>(std::ceil(area.width / Properties::PixelsPerTile())),
                 entityMap.front().getWidth() - corner.x),
        std::min(static_cast<unsigned int>(std::ceil(area.height / Properties::PixelsPerTile())),
                 entityMap.front().getHeight() - corner.y));

    toUpdate.clear();
    toUpdate.reserve(entityMap.size() * size.x * size.y / 4);
    for (const auto& vec2d : entityMap) {
        for (unsigned int x = corner.x; x < corner.x + size.x; ++x) {
            for (unsigned int y = corner.y; y < corner.y + size.y; ++y) {
                const bl::ecs::Entity e = vec2d(x, y);
                if (e != bl::ecs::InvalidEntity) toUpdate.emplace_back(e);
            }
        }
    }*/
}

const std::vector<bl::ecs::Entity>& Position::updateRangeEntities() const { return toUpdate; }

bool Position::spaceFree(const component::Position& position) const {
    if (position.level >= entityMap.size()) return false;
    const auto& level = entityMap[position.level];
    if (static_cast<unsigned>(position.positionTiles().x) >= level.getWidth()) return false;
    if (static_cast<unsigned>(position.positionTiles().y) >= level.getHeight()) return false;

    const bl::ecs::Entity e = level(position.positionTiles().x, position.positionTiles().y);
    return e == bl::ecs::InvalidEntity ||
           !owner.engine().ecs().hasComponent<component::Collision>(e);
}

bl::ecs::Entity Position::search(const component::Position& start, component::Direction dir,
                                 unsigned int range) {
    component::Position spos = start;
    spos.direction           = dir;
    for (unsigned int i = 0; i < range; ++i) {
        const auto oldPos       = spos;
        spos                    = owner.world().activeMap().adjacentTile(spos, dir);
        const bl::ecs::Entity e = get(spos);
        if (e != bl::ecs::InvalidEntity) return e;

        // break after check to allow entity on collision to be found
        if (!owner.world().activeMap().movePossible(oldPos, dir)) break;
    }

    return bl::ecs::InvalidEntity;
}

void Position::observe(const event::EntityMoved& event) {
    get(event.previousPosition) = bl::ecs::InvalidEntity;
    get(event.position)         = event.entity;
}

void Position::observe(const bl::ecs::event::ComponentAdded<component::Position>& event) {
    get(event.component) = event.entity;
}

void Position::observe(const bl::ecs::event::ComponentRemoved<component::Position>& event) {
    get(event.component) = bl::ecs::InvalidEntity;
}

void Position::observe(const event::MapSwitch& event) {
    entityMap.clear();
    entityMap.reserve(event.map.levelCount());
    for (std::uint8_t i = 0; i < event.map.levelCount(); ++i) {
        entityMap.emplace_back(
            event.map.sizeTiles().x, event.map.sizeTiles().y, bl::ecs::InvalidEntity);
    }
}

bl::ecs::Entity& Position::get(const component::Position& p) {
    static bl::ecs::Entity null = bl::ecs::InvalidEntity;

    if (p.level < entityMap.size()) {
        if (p.positionTiles().x >= 0 &&
            p.positionTiles().x < static_cast<int>(entityMap[p.level].getWidth())) {
            if (p.positionTiles().y >= 0 &&
                p.positionTiles().y < static_cast<int>(entityMap[p.level].getHeight())) {
                return entityMap[p.level](p.positionTiles().x, p.positionTiles().y);
            }
        }
    }

    BL_LOG_WARN << "Out of bounds entity check at (" << static_cast<int>(p.level) << ", "
                << p.positionTiles().x << ", " << p.positionTiles().y << ")";
    null = bl::ecs::InvalidEntity;
    return null;
}

bl::ecs::Entity Position::getEntity(const component::Position& p) const {
    if (p.level < entityMap.size()) {
        if (p.positionTiles().x >= 0 &&
            p.positionTiles().x < static_cast<int>(entityMap[p.level].getWidth())) {
            if (p.positionTiles().y >= 0 &&
                p.positionTiles().y < static_cast<int>(entityMap[p.level].getHeight())) {
                return entityMap.at(p.level)(p.positionTiles().x, p.positionTiles().y);
            }
        }
    }

    BL_LOG_WARN << "Out of bounds entity check at (" << static_cast<int>(p.level) << ", "
                << p.positionTiles().x << ", " << p.positionTiles().y << ")";
    return bl::ecs::InvalidEntity;
}

void Position::editorPushLevel() {
    entityMap.emplace_back(
        entityMap.front().getWidth(), entityMap.front().getHeight(), bl::ecs::InvalidEntity);
}

void Position::editorPopLevel() { entityMap.pop_back(); }

} // namespace system
} // namespace core
