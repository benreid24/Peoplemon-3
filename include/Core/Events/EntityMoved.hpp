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
    const sf::Vector2i previousPosition;

    /// The current position of the entity
    const component::Position& position;

    /**
     * @brief Construct a new EntityMoved event
     *
     * @param entity The entity that moved
     * @param oldPos The old position
     * @param pos The current position
     */
    EntityMoved(bl::entity::Entity entity, const sf::Vector2i& oldPos,
                const component::Position& pos)
    : entity(entity)
    , previousPosition(oldPos)
    , position(pos) {}
};

} // namespace event
} // namespace core

#endif
