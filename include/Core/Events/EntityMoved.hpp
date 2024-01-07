#ifndef CORE_EVENTS_ENTITYMOVED_HPP
#define CORE_EVENTS_ENTITYMOVED_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Tilemap/Position.hpp>
#include <SFML/System/Vector2.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired after an entity begins moving from one position to another
 *
 * @ingroup Events
 *
 */
struct EntityMoved {
    /// The entity that moved
    const bl::ecs::Entity entity;

    /// The previous position of the entity
    const bl::tmap::Position previousPosition;

    /// The current position of the entity
    const bl::tmap::Position& position;

    /// Whether or not the entity is running
    const bool running;

    /**
     * @brief Construct a new EntityMoved event
     *
     * @param entity The entity that moved
     * @param oldPos The old position
     * @param pos The current position
     */
    EntityMoved(bl::ecs::Entity entity, const bl::tmap::Position& oldPos,
                const bl::tmap::Position& pos, bool running = false)
    : entity(entity)
    , previousPosition(oldPos)
    , position(pos)
    , running(running) {}
};

/**
 * @brief Fired when an entity completes a move from one tile to another
 *
 * @ingroup Events
 *
 */
struct EntityMoveFinished {
    /// The entity that moved
    const bl::ecs::Entity entity;

    /// The current position of the entity
    const bl::tmap::Position& position;

    /**
     * @brief Construct a new Entity Move Finished event
     *
     * @param entity The entity that completed moving
     * @param position The position the entity is at
     */
    EntityMoveFinished(bl::ecs::Entity entity, const bl::tmap::Position& position)
    : entity(entity)
    , position(position) {}
};

/**
 * @brief Fired when an entity rotates without moving
 *
 * @ingroup Events
 *
 */
struct EntityRotated {
    /// The entity that rotated
    const bl::ecs::Entity entity;

    /// The direction the entity is now facing
    const bl::tmap::Direction faceDir;

    /// The direction the entity was facing
    const bl::tmap::Direction origDir;

    /**
     * @brief Construct a new Entity Rotated event
     *
     * @param e The entity that rotated
     * @param dir The direction it is now facing
     * @param og The original direction it was facing
     */
    EntityRotated(bl::ecs::Entity e, bl::tmap::Direction dir, bl::tmap::Direction og)
    : entity(e)
    , faceDir(dir)
    , origDir(og) {}
};

} // namespace event
} // namespace core

#endif
