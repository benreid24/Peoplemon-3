#include <Core/Systems/Entity.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <Core/Components/Collision.hpp>
#include <Core/Components/Controllable.hpp>
#include <Core/Components/Item.hpp>
#include <Core/Components/NPC.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Files/Conversation.hpp>
#include <Core/Files/NPC.hpp>
#include <Core/Files/Trainer.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Maps/CharacterSpawn.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Entity::Entity(Systems& owner)
: owner(owner) {}

bl::ecs::Entity Entity::spawnCharacter(const map::CharacterSpawn& spawn, map::Map& map) {
    bl::ecs::Entity entity = owner.engine().ecs().createEntity(bl::ecs::Flags::WorldObject);
    std::string animation;
    BL_LOG_DEBUG << "Created character entity " << entity;

    bl::ecs::Cleaner cleaner(owner.engine().ecs(), entity);

    // Common components
    bl::tmap::Position* pos =
        owner.engine().ecs().addComponent<bl::tmap::Position>(entity, spawn.position);
    owner.engine().ecs().addComponent<component::Collision>(entity, {});
    component::Movable* mover = owner.engine().ecs().addComponent<component::Movable>(
        entity, {*pos, Properties::CharacterMoveSpeed(), 0.f});
    owner.engine().ecs().addComponent<component::Controllable>(entity, {owner, entity});

    /// NPC
    if (bl::util::FileUtil::getExtension(spawn.file) == Properties::NpcFileExtension()) {
        file::NPC data;
        if (!data.load(bl::util::FileUtil::joinPath(Properties::NpcPath(), spawn.file),
                       spawn.position.direction)) {
            BL_LOG_ERROR << "Failed to load NPC: " << spawn.file;
            return bl::ecs::InvalidEntity;
        }
        BL_LOG_INFO << "Spawning NPC " << data.name() << " at ("
                    << static_cast<int>(spawn.position.level) << ", " << spawn.position.position.x
                    << ", " << spawn.position.position.y << ")";

        if (!owner.ai().addBehavior(entity, data.behavior())) {
            BL_LOG_ERROR << "Failed to add behavior to spawned npc: " << entity;
            return bl::ecs::InvalidEntity;
        }

        animation = data.animation();
        owner.engine().ecs().addComponent<component::NPC>(entity, component::NPC(data));
    }

    // Trainer
    else if (bl::util::FileUtil::getExtension(spawn.file) == Properties::TrainerFileExtension()) {
        file::Trainer data;
        if (!data.load(bl::util::FileUtil::joinPath(Properties::TrainerPath(), spawn.file),
                       spawn.position.direction)) {
            BL_LOG_ERROR << "Failed to load trainer: " << spawn.file;
            return bl::ecs::InvalidEntity;
        }
        BL_LOG_INFO << "Spawning trainer " << data.name << " at ("
                    << static_cast<int>(spawn.position.level) << ", " << spawn.position.position.x
                    << ", " << spawn.position.position.y << ")";

        if (!owner.ai().addBehavior(entity, data.behavior)) {
            BL_LOG_ERROR << "Failed to add behavior to spawned npc: " << entity;
            return bl::ecs::InvalidEntity;
        }

        animation = data.animation;
        owner.engine().ecs().emplaceComponent<component::Trainer>(entity, std::move(data));
    }
    else {
        BL_LOG_ERROR << "Unknown character file type: " << spawn.file;
        return bl::ecs::InvalidEntity;
    }

    /// More common components
    component::Renderable::createFromMoveAnims(
        owner.engine(),
        entity,
        map.getScene(),
        bl::util::FileUtil::joinPath(Properties::CharacterAnimationPath(), animation));
    map.setupEntityPosition(entity);

    cleaner.disarm();
    return entity;
}

bool Entity::spawnItem(const map::Item& item, map::Map& map) {
    const item::Id id = item::Item::cast(item.id);
    BL_LOG_INFO << "Spawning item " << item.id << " (" << item::Item::getName(id) << ") at ("
                << item.position.x << " , " << item.position.y << ")";
    if (id == item::Id::Unknown) {
        BL_LOG_ERROR << "Unknown item id: " << item.id;
        return false;
    }

    const bl::ecs::Entity entity = owner.engine().ecs().createEntity(bl::ecs::Flags::WorldObject);

    bl::tmap::Position* pos = owner.engine().ecs().emplaceComponent<bl::tmap::Position>(
        entity,
        item.level,
        glm::i32vec2(item.position.x, item.position.y),
        bl::tmap::Direction::Up);
    owner.engine().ecs().addComponent<component::Item>(entity, {id});

    if (item.visible || Properties::InEditor()) {
        owner.engine().ecs().addComponent<component::Collision>(entity, {});
        component::Renderable::createFromSprite(
            owner.engine(), entity, map.getScene(), Properties::ItemSprite());
        map.setupEntityPosition(entity);
    }

    return true;
}

bl::ecs::Entity Entity::spawnGeneric(std::uint8_t level, const glm::i32vec2& tiles, bool collidable,
                                     const std::string& gfx, map::Map& map) {
    bl::ecs::Entity entity = owner.engine().ecs().createEntity(bl::ecs::Flags::WorldObject);
    bl::ecs::Cleaner cleaner(owner.engine().ecs(), entity);
    BL_LOG_DEBUG << "Created generic entity " << entity;

    bl::tmap::Position* pos = owner.engine().ecs().emplaceComponent<bl::tmap::Position>(
        entity, level, tiles, bl::tmap::Direction::Up);
    const bool isAnim = bl::util::FileUtil::getExtension(gfx) == "anim";
    if (isAnim) {
        component::Renderable::createFromAnimation(owner.engine(), entity, map.getScene(), gfx);
    }
    else {
        component::Renderable::createFromSprite(
            owner.engine(),
            entity,
            map.getScene(),
            bl::util::FileUtil::joinPath(Properties::ImagePath(), gfx));
    }

    map.setupEntityPosition(entity);

    if (collidable) { owner.engine().ecs().addComponent<component::Collision>(entity, {}); }

    cleaner.disarm();
    return entity;
}

bl::ecs::Entity Entity::spawnAnimation(std::uint8_t level, const glm::vec2& worldPos,
                                       const std::string& gfx, map::Map& map) {
    bl::ecs::Entity entity = owner.engine().ecs().createEntity(bl::ecs::Flags::WorldObject);
    bl::ecs::Cleaner cleaner(owner.engine().ecs(), entity);
    BL_LOG_DEBUG << "Created animation entity " << entity;

    component::Renderable::createFromAnimation(owner.engine(), entity, map.getScene(), gfx);
    // TODO - BLIB_UPGRADE - set depth based on pos

    cleaner.disarm();
    return entity;
}

} // namespace system
} // namespace core
