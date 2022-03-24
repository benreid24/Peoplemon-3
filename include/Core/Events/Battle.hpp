#ifndef CORE_EVENTS_BATTLE_HPP
#define CORE_EVENTS_BATTLE_HPP

#include <Core/Battles/Battle.hpp>

namespace core
{
namespace event
{
/**
 * @brief Fired when a battle should start. This does not actually start a battle, but communicates
 *        to the current game state that a battle should begin
 *
 * @ingroup Events
 *
 */
struct BattleStarted {
    /// The battle to start
    std::unique_ptr<battle::Battle> battle;

    /**
     * @brief Construct a new Battle Started event
     *
     * @param battle The battle to start
     */
    BattleStarted(std::unique_ptr<battle::Battle>&& battle)
    : battle(std::forward<std::unique_ptr<battle::Battle>>(battle)) {}
};

/**
 * @brief Fired when a battle finishes
 *
 * @ingroup Events
 *
 */
struct BattleCompleted {
    /**
     * @brief Construct a new Battle Completed event
     *
     * @param type The type of battle that was completed
     * @param playerwon Whether or not the local player won
     */
    BattleCompleted(battle::Battle::Type type, bool playerwon)
    : type(type)
    , playerWon(playerwon) {}

    /// The type of battle that was completed
    const battle::Battle::Type type;

    /// Whether or not the local player won
    const bool playerWon;
};

} // namespace event
} // namespace core

#endif
