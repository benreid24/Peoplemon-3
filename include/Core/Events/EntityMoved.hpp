#ifndef CORE_EVENTS_ENTITYMOVED_HPP
#define CORE_EVENTS_ENTITYMOVED_HPP

#include <BLIB/Entities/Entity.hpp>
#include <Core/Components/Position.hpp>
#include <SFML/System/Vector2.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired after an entity completed moving from one position to another
 *
 * @ingroup Events
 *
 */
struct EntityMoved {
    /// The entity that moved
    const bl::entity::Entity entity;

    /// The previous position of the entity
    const component::Position previousPosition;

    /// The current position of the entity
    const component::Position& position;

    /**
     * @brief Construct a new EntityMoved event
     *
     * @param entity The entity that moved
     * @param oldPos The old position
     * @param pos The current position
     */
    EntityMoved(bl::entity::Entity entity, const component::Position& oldPos,
                const component::Position& pos)
    : entity(entity)
    , previousPosition(oldPos)
    , position(pos) {}
};

/**
 * @brief Fired when an entity rotates without moving
 *
 * @ingroup Events
 *
 */
struct EntityRotated {
    /// The entity that rotated
    const bl::entity::Entity entity;

    /// The direction the entity is now facing
    const component::Direction faceDir;

    /// The direction the entity was facing
    const component::Direction origDir;

    /**
     * @brief Construct a new Entity Rotated event
     *
     * @param e The entity that rotated
     * @param dir The direction it is now facing
     * @param og The original direction it was facing
     */
    EntityRotated(bl::entity::Entity e, component::Direction dir, component::Direction og)
    : entity(e)
    , faceDir(dir)
    , origDir(og) {}
};

} // namespace event
} // namespace core

#endif
