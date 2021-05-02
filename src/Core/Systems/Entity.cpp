#include <Core/Systems/Entity.hpp>

#include <BLIB/Files/Util.hpp>
#include <Core/Components/Collision.hpp>
#include <Core/Files/NPC.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Entity::Entity(Systems& owner)
: owner(owner) {}

bool Entity::spawnCharacter(const map::CharacterSpawn& spawn) {
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

        bl::entity::Entity npc = owner.engine().entities().createEntity();
        BL_LOG_DEBUG << "Created NPC entity " << npc;

        if (!owner.engine().entities().addComponent<component::Position>(npc, spawn.position)) {
            BL_LOG_ERROR << "Failed to add position component to NPC: " << npc;
            return false;
        }

        if (!owner.engine().entities().addComponent<component::Collision>(npc, {})) {
            BL_LOG_ERROR << "Failed to add collision component to NPC: " << npc;
            return false;
        }

        auto posHandle = owner.engine().entities().getComponentHandle<component::Position>(npc);
        if (!posHandle.hasValue()) {
            BL_LOG_ERROR << "Failed to get position component handle for NPC: " << npc;
            return false;
        }

        if (!owner.engine().entities().addComponent<component::Movable>(
                npc, {posHandle, Properties::CharacterMoveSpeed(), 0.f})) {
            BL_LOG_ERROR << "Failed to add movable component to NPC: " << npc;
            return false;
        }

        auto moveHandle = owner.engine().entities().getComponentHandle<component::Movable>(npc);
        if (!moveHandle.hasValue()) {
            BL_LOG_ERROR << "Failed to get movable handle for NPC: " << npc;
            return false;
        }

        if (!owner.engine().entities().addComponent<component::Renderable>(
                npc,
                component::Renderable::fromMoveAnims(
                    posHandle,
                    moveHandle,
                    bl::file::Util::joinPath(Properties::CharacterAnimationPath(),
                                             data.animation())))) {
            BL_LOG_ERROR << "Failed to add renderable component to NPC: " << npc;
            return false;
        }
        // TODO - other components like conversation
        return true;
    }
    else if (bl::file::Util::getExtension(spawn.file) == Properties::TrainerFileExtension()) {
        // TODO
        return false;
    }
    else {
        BL_LOG_ERROR << "Unknown character file type: " << spawn.file.getValue();
        return false;
    }
}

void Entity::spawnItem(const map::Item& item) {
    // TODO
}

} // namespace system
} // namespace core
