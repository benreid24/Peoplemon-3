#ifndef CORE_SYSTEMS_MOVEMENT_HPP
#define CORE_SYSTEMS_MOVEMENT_HPP

#include <BLIB/Audio/AudioSystem.hpp>
#include <BLIB/ECS.hpp>
#include <Core/Components/Movable.hpp>
#include <BLIB/Tilemap/Position.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Primary System for managing entity movement and interpolation
 *
 * @ingroup Systems
 */
class Movement {
public:
    /**
     * @brief Create the movement system
     *
     * @param owner The primary systems object
     */
    Movement(Systems& owner);

    /**
     * @brief Adds a Movable component to the given entity if it does not already exist
     *
     * @param entity The entity to make movable
     * @param moveSpeed The speed to move at
     * @param fastMoveSpeed The speed to move at when moving fast
     * @return True if a component was added, false otherwise
     */
    bool makeMovable(bl::ecs::Entity entity, float moveSpeed, float fastMoveSpeed);

    /**
     * @brief Moves an entity in the given direction using either its fast or slow speed.
     *
     * @param entity The entity to move
     * @param direction The direction to move in
     * @param fast True to move at the fast speed, false for slow
     * @return True if the movement is possible, false if no movement will occur
     */
    bool moveEntity(bl::ecs::Entity entity, bl::tmap::Direction direction, bool fast);

    /**
     * @brief Updates all moving entities
     *
     * @param dt Time elapsed in seconds since the last call to update
     */
    void update(float dt);

private:
    Systems& owner;
    bl::audio::AudioSystem::Handle jumpSound;
};

} // namespace system
} // namespace core

#endif
