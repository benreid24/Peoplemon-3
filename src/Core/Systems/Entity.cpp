#include <Core/Systems/Entity.hpp>

#include <BLIB/Files/Util.hpp>
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
        if (!data.load(spawn.file)) {
            BL_LOG_ERROR << "Failed to load NPC: " << spawn.file.getValue();
            return false;
        }
        // TODO - build components
        return false;
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
