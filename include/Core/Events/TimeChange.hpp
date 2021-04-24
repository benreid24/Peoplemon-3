#ifndef CORE_EVENTS_CLOCKEVENT_HPP
#define CORE_EVENTS_CLOCKEVENT_HPP

#include <Core/Systems/Clock.hpp>

/**
 * @addtogroup Events
 * @ingroup Core
 * @brief Collection of event types that are propagated through the engine event bus
 *
 */

namespace core
{
/// Namespace containing all the event types that propagate through the event bus
namespace event
{
/**
 * @brief Basic event for when the game time changes
 *
 * @ingroup Events
 *
 */
struct TimeChange {
    /**
     * @brief Creates a new time change event
     *
     * @param time The new time
     */
    TimeChange(const system::Clock::Time& time)
    : newTime(time) {}

    /// A reference to the new time
    const system::Clock::Time& newTime;
};

} // namespace event
} // namespace core

#endif
