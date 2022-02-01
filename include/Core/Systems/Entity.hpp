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
     * @return Id of the created entity, or InvalidId if spawn failed
     */
    bl::entity::Entity spawnCharacter(const map::CharacterSpawn& spawn);

    /**
     * @brief Spawns an item into the world
     *
     * @param item The item information to spawn
     * @return True if the item was spawned, false on error
     */
    bool spawnItem(const map::Item& item);

    /**
     * @brief Spawns a generic entity with some basic components
     *
     * @param position The position to spawn at
     * @param collidable True to add a collidable component, false to not
     * @param gfx Path to image or animation for graphic component
     * @return The created entity id or InvalidEntity
     */
    bl::entity::Entity spawnGeneric(const component::Position& position, bool collidable,
                                    const std::string& gfx);

    /**
     * @brief Spawns an animation-only entity at the given position. The animation is not played
     *        until manually triggered
     *
     * @param position The position to spawn the entity at
     * @param gfx The path to the animation to render
     * @param center True to center the animation, false for top left origin
     * @return The created entity id or InvalidEntity
     */
    bl::entity::Entity spawnAnimation(const component::Position& position, const std::string& gfx,
                                      bool center);

private:
    Systems& owner;
};

} // namespace system
} // namespace core

#endif
