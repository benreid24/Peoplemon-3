#ifndef CORE_SYSTEMS_PLAYER_HPP
#define CORE_SYSTEMS_PLAYER_HPP

#include <BLIB/Entities.hpp>
#include <Core/Components/Movable.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Player/Gender.hpp>
#include <Core/Player/Input.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Primary system for managing the player and their data
 *
 * @ingroup Systems
 *
 */
class Player {
public:
    /**
     * @brief Construct a new Player system
     *
     * @param owner The primary systems object
     */
    Player(Systems& owner);

    /**
     * @brief Spawns the player into the world
     *
     * @param position The position to spawn at
     * @return True if the player was spawned, false on error
     */
    bool spawnPlayer(const component::Position& position);

    /**
     * @brief Makes the given entity controlled by the player. Only one entity may be player
     *        controlled at a time
     *
     * @param entity The entity to receive player input
     * @return True if added, false on error
     */
    bool makePlayerControlled(bl::entity::Entity entity);

    /**
     * @brief Removes the PlayerControlled component from the given entity, if any
     *
     * @param entity The entity to remove player control from
     */
    void removePlayerControlled(bl::entity::Entity entity);

    /**
     * @brief Returns the id of the player entity
     *
     */
    bl::entity::Entity player() const;

    /**
     * @brief Returns a reference to the player input system
     *
     */
    player::Input& inputSystem();

    /**
     * @brief Subscribes the input system to the event bus
     *
     */
    void init();

    /**
     * @brief Updates the player system
     *
     */
    void update();

private:
    Systems& owner;
    bl::entity::Entity playerId;
    bl::entity::Registry::ComponentHandle<component::Position> position;
    bl::entity::Registry::ComponentHandle<component::Movable> movable;

    player::Input input;
    player::Gender gender;
    // TODO - other stuff like inventory, peoplemon, etc
};

} // namespace system
} // namespace core

#endif
