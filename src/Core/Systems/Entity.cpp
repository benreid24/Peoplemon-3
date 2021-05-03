#include <Core/Systems/Entity.hpp>

#include <BLIB/Files/Util.hpp>
#include <Core/Components/Collision.hpp>
#include <Core/Files/NPC.hpp>
#include <Core/Files/Trainer.hpp>
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

    if (bl::file::Util::getExtension(spawn.file) == Properties::NpcFileExtension()) {
        file::NPC data;
        if (!data.load(bl::file::Util::joinPath(Properties::NpcPath(), spawn.file))) {
            BL_LOG_ERROR << "Failed to load NPC: " << spawn.file.getValue();
            return false;
        }
        BL_LOG_INFO << "Spawning NPC " << data.name() << " at ("
                    << static_cast<int>(spawn.position.getValue().level) << ", "
                    << spawn.position.getValue().positionTiles().x << ", "
                    << spawn.position.getValue().positionTiles().y << ")";

        // TODO - components like conversation
        animation = data.animation();
    }
    else if (bl::file::Util::getExtension(spawn.file) == Properties::TrainerFileExtension()) {
        file::Trainer data;
        if (!data.load(bl::file::Util::joinPath(Properties::TrainerPath(), spawn.file))) {
            BL_LOG_ERROR << "Failed to load trainer: " << spawn.file.getValue();
            return false;
        }
        BL_LOG_INFO << "Spawning trainer " << data.name() << " at ("
                    << static_cast<int>(spawn.position.getValue().level) << ", "
                    << spawn.position.getValue().positionTiles().x << ", "
                    << spawn.position.getValue().positionTiles().y << ")";

        // TODO - components like peoplemon, conversation, items
        animation = data.animation();
    }
    else {
        BL_LOG_ERROR << "Unknown character file type: " << spawn.file.getValue();
        return false;
    }

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

    if (!owner.engine().entities().addComponent<component::Renderable>(
            entity,
            component::Renderable::fromMoveAnims(
                posHandle,
                moveHandle,
                bl::file::Util::joinPath(Properties::CharacterAnimationPath(), animation)))) {
        BL_LOG_ERROR << "Failed to add renderable component to character: " << entity;
        return false;
    }

    return true;
}

void Entity::spawnItem(const map::Item& item) {
    // TODO
}

} // namespace system
} // namespace core
