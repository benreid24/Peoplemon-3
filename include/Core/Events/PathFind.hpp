#ifndef CORE_EVENTS_PATHFIND_HPP
#define CORE_EVENTS_PATHFIND_HPP

#include <BLIB/ECS/Entity.hpp>

namespace core
{
namespace event
{
/**
 * @brief Event that is fired when an entity path find is completed
 *
 * @ingroup Events
 *
 */
struct PathFindCompleted {
    /**
     * @brief Construct a new Path Find Completed event
     *
     * @param entity The entity that finished
     * @param success True if the destination was reached, false otherwise
     */
    PathFindCompleted(bl::ecs::Entity entity, bool success)
    : entity(entity)
    , success(success) {}

    const bl::ecs::Entity entity;
    bool success;
};

} // namespace event
} // namespace core

#endif
