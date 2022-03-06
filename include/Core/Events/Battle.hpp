#ifndef CORE_EVENTS_BATTLE_HPP
#define CORE_EVENTS_BATTLE_HPP

namespace core
{
namespace event
{
/**
 * @brief Fired when a battle finishes
 *
 * @ingroup Events
 *
 */
struct BattleCompleted {
    /// The type of battle that was completed
    enum Type { WildPeoplemon, Trainer, Network };

    /**
     * @brief Construct a new Battle Completed event
     *
     * @param type The type of battle that was completed
     */
    BattleCompleted(Type type)
    : type(type) {}

    /// The type of battle that was completed
    const Type type;
};

} // namespace event
} // namespace core

#endif
