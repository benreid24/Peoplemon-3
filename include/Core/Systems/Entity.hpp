#ifndef CORE_SYSTEMS_ENTITY_HPP
#define CORE_SYSTEMS_ENTITY_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
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
     * @param scene The scene to spawn in
     * @return Id of the created entity, or InvalidId if spawn failed
     */
    bl::ecs::Entity spawnCharacter(const map::CharacterSpawn& spawn, bl::rc::Scene* scene);

    /**
     * @brief Spawns an item into the world
     *
     * @param item The item information to spawn
     * @param scene The scene to spawn in
     * @return True if the item was spawned, false on error
     */
    bool spawnItem(const map::Item& item, bl::rc::Scene* scene);

    /**
     * @brief Spawns a generic entity with some basic components
     *
     * @param level The map level to spawn on
     * @param tiles The position to spawn at
     * @param collidable True to add a collidable component, false to not
     * @param gfx Path to image or animation for graphic component
     * @param scene The scene to spawn in
     * @return The created entity id or InvalidEntity
     */
    bl::ecs::Entity spawnGeneric(std::uint8_t level, const glm::i32vec2& tiles, bool collidable,
                                 const std::string& gfx, bl::rc::Scene* scene);

    /**
     * @brief Spawns an animation-only entity at the given position. The animation is not played
     *        until manually triggered
     *
     * @param level The map level to spawn on
     * @param position The position to spawn the entity at
     * @param gfx The path to the animation to render
     * @param scene The scene to spawn in
     * @return The created entity id or InvalidEntity
     */
    bl::ecs::Entity spawnAnimation(std::uint8_t level, const glm::vec2& position,
                                   const std::string& gfx, bl::rc::Scene* scene);

private:
    Systems& owner;
};

} // namespace system
} // namespace core

#endif
