#include <Core/Systems/Entity.hpp>

#include <BLIB/Files/Util.hpp>
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

bool Entity::spawnCharacter(const map::CharacterSpawn& spawn) {
    bl::entity::Entity entity = owner.engine().entities().createEntity();
    std::string animation;
    BL_LOG_DEBUG << "Created character entity " << entity;

    bl::entity::Cleaner cleaner(owner.engine().entities(), entity);

    // Common components

    if (!owner.engine().entities().addComponent<component::Position>(entity, spawn.position)) {
        BL_LOG_ERROR << "Failed to add position component to character: " << entity;
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Collision>(entity, {})) {
        BL_LOG_ERROR << "Failed to add collision component to character: " << entity;
        return false;
    }

    auto posHandle = owner.engine().entities().getComponentHandle<component::Position>(entity);
    if (!posHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get position component handle for character: " << entity;
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Movable>(
            entity, {posHandle, Properties::CharacterMoveSpeed(), 0.f})) {
        BL_LOG_ERROR << "Failed to add movable component to character: " << entity;
        return false;
    }

    auto moveHandle = owner.engine().entities().getComponentHandle<component::Movable>(entity);
    if (!moveHandle.hasValue()) {
        BL_LOG_ERROR << "Failed to get movable handle for character: " << entity;
        return false;
    }

    if (!owner.engine().entities().addComponent<component::Controllable>(entity, {owner, entity})) {
        BL_LOG_ERROR << "Failed to add Controllable component to character: " << entity;
        return false;
    }

    /// NPC
    if (bl::file::Util::getExtension(spawn.file) == Properties::NpcFileExtension()) {
        file::NPC data;
        if (!data.load(bl::file::Util::joinPath(Properties::NpcPath(), spawn.file),
                       spawn.position.getValue().direction)) {
            BL_LOG_ERROR << "Failed to load NPC: " << spawn.file.getValue();
            return false;
        }
        BL_LOG_INFO << "Spawning NPC " << data.name() << " at ("
                    << static_cast<int>(spawn.position.getValue().level) << ", "
                    << spawn.position.getValue().positionTiles().x << ", "
                    << spawn.position.getValue().positionTiles().y << ")";

        if (!owner.ai().addBehavior(entity, data.behavior())) {
            BL_LOG_ERROR << "Failed to add behavior to spawned npc: " << entity;
            return false;
        }

        animation = data.animation();

        /*if (!owner.engine().entities().addComponent<component::NPC>(entity, component::NPC(data)))
        { BL_LOG_ERROR << "Failed to add NPC component to npc: " << entity; return false;
        }*/
    }

    // Trainer
    else if (bl::file::Util::getExtension(spawn.file) == Properties::TrainerFileExtension()) {
        file::Trainer data;
        if (!data.load(bl::file::Util::joinPath(Properties::TrainerPath(), spawn.file),
                       spawn.position.getValue().direction)) {
            BL_LOG_ERROR << "Failed to load trainer: " << spawn.file.getValue();
            return false;
        }
        BL_LOG_INFO << "Spawning trainer " << data.name() << " at ("
                    << static_cast<int>(spawn.position.getValue().level) << ", "
                    << spawn.position.getValue().positionTiles().x << ", "
                    << spawn.position.getValue().positionTiles().y << ")";

        if (!owner.ai().addBehavior(entity, data.behavior())) {
            BL_LOG_ERROR << "Failed to add behavior to spawned npc: " << entity;
            return false;
        }

        animation = data.animation();

        /*if (!owner.engine().entities().addComponent<component::Trainer>(entity,
                                                                        component::Trainer(data))) {
            BL_LOG_ERROR << "Failed to add trainer component to entity: " << entity;
            return false;
        }*/
    }
    else {
        BL_LOG_ERROR << "Unknown character file type: " << spawn.file.getValue();
        return false;
    }

    /// More common components

    if (!owner.engine().entities().addComponent<component::Renderable>(
            entity,
            component::Renderable::fromMoveAnims(
                posHandle,
                moveHandle,
                bl::file::Util::joinPath(Properties::CharacterAnimationPath(), animation)))) {
        BL_LOG_ERROR << "Failed to add renderable component to character: " << entity;
        return false;
    }

    cleaner.disarm();
    return true;
}

bool Entity::spawnItem(const map::Item& item) {
    const item::Id id = item::Item::cast(item.id);
    BL_LOG_INFO << "Spawning item " << item.id.getValue() << " (" << item::Item::getName(id)
                << ") at (" << item.position.getValue().x << " , " << item.position.getValue().y
                << ")";
    if (id == item::Id::Unknown) {
        BL_LOG_ERROR << "Unknown item id: " << item.id.getValue();
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

    if (item.visible) {
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

} // namespace system
} // namespace core
