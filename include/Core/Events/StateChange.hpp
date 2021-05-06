#ifndef CORE_EVENTS_STATECHANGE_HPP
#define CORE_EVENTS_STATECHANGE_HPP

namespace core
{
namespace event
{
/**
 * @brief Fired when the game should switch states. This event type is primarily for decoupling
 *        input code in Core from state code in Game
 *
 * @ingroup Events
 *
 */
struct StateChange {
    /// The type of state change that should occur
    enum Type {
        /// The game should pause
        GamePaused = 0,

        /// Context dependent action
        BackPressed = 1
    };

    /// The type of state change that should occur
    const Type type;

    /**
     * @brief Construct a new State Change event
     *
     * @param t The type of state change
     */
    StateChange(Type t)
    : type(t) {}
};

} // namespace event
} // namespace core

#endif
