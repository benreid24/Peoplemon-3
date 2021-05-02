#ifndef CORE_SYSTEMS_ENTITY_HPP
#define CORE_SYSTEMS_ENTITY_HPP

#include <Core/Maps/CharacterSpawn.hpp>
#include <Core/Maps/Item.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Basic helper system for creating entities and their required components. Also manages
 *        player data and entity id
 *
 * @ingroup Systems
 *
 */
class Entity {
public:
    /**
     * @brief Construct a new Entity system
     *
     * @param owner The primary systems object
     */
    Entity(Systems& owner);

    /**
     * @brief Spawns a trainer or an npc from the given spawn information
     *
     * @param spawn The map spawn information
     * @return True if the spawn is valid and an entity was created, false on error
     */
    bool spawnCharacter(const map::CharacterSpawn& spawn);

    /**
     * @brief Spawns an item into the world
     *
     * @param item The item information to spawn
     */
    void spawnItem(const map::Item& item);

    // TODO - methods for player

private:
    Systems& owner;
};

} // namespace system
} // namespace core

#endif
