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
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Entity::Entity(Systems& owner)
: owner(owner) {}

bl::entity::Entity Entity::spawnCharacter(const map::CharacterSpawn& spawn) {
    bl::entity::Entity entity = owner.engine().entities().createEntity();
    std::string animation;
    BL_LOG_DEBUG << "Created character entity " << entity;

    bl::entity::Cleaner cleaner(owner.engine().entities(), entity);

    // Common components

    if (!owner.engine().entities().addComponent<component::Position>(entity, spawn.position)) {
        BL_LOG_ERROR << "Failed to add position component to character: " << entity;
        return bl::entity::InvalidEntity;
    }

    if (!owner.engine().entities().addComponent<component::Collision>(entity, {})) {
        BL_LOG_ERROR << "Failed to add collision component to character: " << entity;
        return bl::entity::InvalidEntity;
    }

    auto posHandle = owner.engine().entities().getComponentHandle<component::Position>(entity);
    if (!posHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get position component handle for character: " << entity;
        return bl::entity::InvalidEntity;
    }

    if (!owner.engine().entities().addComponent<component::Movable>(
            entity, {posHandle, Properties::CharacterMoveSpeed(), 0.f})) {
        BL_LOG_ERROR << "Failed to add movable component to character: " << entity;
        return bl::entity::InvalidEntity;
    }

    auto moveHandle = owner.engine().entities().getComponentHandle<component::Movable>(entity);
    if (!moveHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get movable handle for character: " << entity;
        return bl::entity::InvalidEntity;
    }

    if (!owner.engine().entities().addComponent<component::Controllable>(entity, {owner, entity})) {
        BL_LOG_ERROR << "Failed to add Controllable component to character: " << entity;
        return bl::entity::InvalidEntity;
    }

    /// NPC
    if (bl::util::FileUtil::getExtension(spawn.file) == Properties::NpcFileExtension()) {
        file::NPC data;
        if (!data.load(bl::util::FileUtil::joinPath(Properties::NpcPath(), spawn.file),
                       spawn.position.direction)) {
            BL_LOG_ERROR << "Failed to load NPC: " << spawn.file;
            return bl::entity::InvalidEntity;
        }
        BL_LOG_INFO << "Spawning NPC " << data.name() << " at ("
                    << static_cast<int>(spawn.position.level) << ", "
                    << spawn.position.positionTiles().x << ", " << spawn.position.positionTiles().y
                    << ")";

        if (!owner.ai().addBehavior(entity, data.behavior())) {
            BL_LOG_ERROR << "Failed to add behavior to spawned npc: " << entity;
            return bl::entity::InvalidEntity;
        }

        animation = data.animation();

        if (!owner.engine().entities().addComponent<component::NPC>(entity, component::NPC(data))) {
            BL_LOG_ERROR << "Failed to add NPC component to npc: " << entity;
            return false;
        }
    }

    // Trainer
    else if (bl::util::FileUtil::getExtension(spawn.file) == Properties::TrainerFileExtension()) {
        file::Trainer data;
        if (!data.load(bl::util::FileUtil::joinPath(Properties::TrainerPath(), spawn.file),
                       spawn.position.direction)) {
            BL_LOG_ERROR << "Failed to load trainer: " << spawn.file;
            return bl::entity::InvalidEntity;
        }
        BL_LOG_INFO << "Spawning trainer " << data.name << " at ("
                    << static_cast<int>(spawn.position.level) << ", "
                    << spawn.position.positionTiles().x << ", " << spawn.position.positionTiles().y
                    << ")";

        if (!owner.ai().addBehavior(entity, data.behavior)) {
            BL_LOG_ERROR << "Failed to add behavior to spawned npc: " << entity;
            return bl::entity::InvalidEntity;
        }

        animation = data.animation;

        if (!owner.engine().entities().addComponent<component::Trainer>(entity,
                                                                        component::Trainer(data))) {
            BL_LOG_ERROR << "Failed to add trainer component to entity: " << entity;
            return false;
        }
    }
    else {
        BL_LOG_ERROR << "Unknown character file type: " << spawn.file;
        return bl::entity::InvalidEntity;
    }

    /// More common components

    if (!owner.engine().entities().addComponent<component::Renderable>(
            entity,
            component::Renderable::fromMoveAnims(
                posHandle,
                moveHandle,
                bl::util::FileUtil::joinPath(Properties::CharacterAnimationPath(), animation)))) {
        BL_LOG_ERROR << "Failed to add renderable component to character: " << entity;
        return bl::entity::InvalidEntity;
    }

    cleaner.disarm();
    return entity;
}

bool Entity::spawnItem(const map::Item& item) {
    const item::Id id = item::Item::cast(item.id);
    BL_LOG_INFO << "Spawning item " << item.id << " (" << item::Item::getName(id) << ") at ("
                << item.position.x << " , " << item.position.y << ")";
    if (id == item::Id::Unknown) {
        BL_LOG_ERROR << "Unknown item id: " << item.id;
        return false;
    }

    const bl::entity::Entity entity = owner.engine().entities().createEntity();

    if (!owner.engine().entities().addComponent<component::Position>(
            entity, {item.level, item.position, component::Direction::Up})) {
        BL_LOG_ERROR << "Failed to add position to item entity: " << entity;
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Item>(entity, {id})) {
        BL_LOG_ERROR << "Failed to add item component to item entity: " << entity;
        return false;
    }

    if (item.visible || Properties::InEditor()) {
        if (!owner.engine().entities().addComponent<component::Collision>(entity, {})) {
            BL_LOG_ERROR << "Failed to add collision to item entity: " << entity;
            return false;
        }

        auto pos = owner.engine().entities().getComponentHandle<component::Position>(entity);
        if (!pos.hasValue()) {
            BL_LOG_ERROR << "Failed to get position handle for item entity: " << entity;
            return false;
        }

        if (!owner.engine().entities().addComponent<component::Renderable>(
                entity, component::Renderable::fromSprite(pos, Properties::ItemSprite()))) {
            BL_LOG_ERROR << "Failed to add renderable component to item entity: " << entity;
            return false;
        }
    }

    return true;
}

bl::entity::Entity Entity::spawnGeneric(const component::Position& position, bool collidable,
                                        const std::string& gfx) {
    bl::entity::Entity entity = owner.engine().entities().createEntity();
    bl::entity::Cleaner cleaner(owner.engine().entities(), entity);
    BL_LOG_DEBUG << "Created generic entity " << entity;

    if (!owner.engine().entities().addComponent<component::Position>(entity, position)) {
        BL_LOG_ERROR << "Failed to add position component to entity: " << entity;
        return bl::entity::InvalidEntity;
    }

    auto posHandle = owner.engine().entities().getComponentHandle<component::Position>(entity);
    if (!posHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get position component handle for entity: " << entity;
        return bl::entity::InvalidEntity;
    }

    const bool isAnim = bl::util::FileUtil::getExtension(gfx) == "anim";
    if (!owner.engine().entities().addComponent<component::Renderable>(
            entity,
            isAnim ? component::Renderable::fromAnimation(posHandle, gfx) :
                     component::Renderable::fromSprite(
                         posHandle, bl::util::FileUtil::joinPath(Properties::ImagePath(), gfx)))) {
        BL_LOG_ERROR << "Failed to add renderable component to entity: " << entity;
        return bl::entity::InvalidEntity;
    }

    if (collidable) {
        if (!owner.engine().entities().addComponent<component::Collision>(entity, {})) {
            BL_LOG_ERROR << "Failed to add collision to generic entity: " << entity;
            return bl::entity::InvalidEntity;
        }
    }

    cleaner.disarm();
    return entity;
}

bl::entity::Entity Entity::spawnAnimation(const component::Position& position,
                                          const std::string& gfx) {
    bl::entity::Entity entity = owner.engine().entities().createEntity();
    bl::entity::Cleaner cleaner(owner.engine().entities(), entity);
    BL_LOG_DEBUG << "Created animation entity " << entity;

    if (!owner.engine().entities().addComponent<component::Position>(entity, position)) {
        BL_LOG_ERROR << "Failed to add position component to animation: " << entity;
        return bl::entity::InvalidEntity;
    }

    auto posHandle = owner.engine().entities().getComponentHandle<component::Position>(entity);
    if (!posHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get position component handle for entity: " << entity;
        return bl::entity::InvalidEntity;
    }

    const sf::Vector2f tx(posHandle.get().positionTiles());
    posHandle.get().setPixels(tx * 32.f + posHandle.get().positionPixels());

    if (!owner.engine().entities().addComponent<component::Renderable>(
            entity, component::Renderable::fromAnimation(posHandle, gfx))) {
        BL_LOG_ERROR << "Failed to add renderable component to entity: " << entity;
        return bl::entity::InvalidEntity;
    }

    cleaner.disarm();
    return entity;
}

} // namespace system
} // namespace core
