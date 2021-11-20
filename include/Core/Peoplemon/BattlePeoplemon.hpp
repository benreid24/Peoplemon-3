#ifndef CORE_PEOPLEMON_BATTLEPEOPLEMON_HPP
#define CORE_PEOPLEMON_BATTLEPEOPLEMON_HPP

#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Peoplemon/PassiveAilment.hpp>
#include <Core/Peoplemon/SpecialAbility.hpp>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a Peoplemon in battle. Stores some extra state that only exists in battle.
 *        Wraps an underlying OwnedPeoplemon
 *
 * @ingroup Peoplemon
 *
 */
class BattlePeoplemon {
public:
    /**
     * @brief Construct a new Battle Peoplemon
     *
     * @param peoplemon The peoplemon to wrap
     */
    BattlePeoplemon(OwnedPeoplemon* peoplemon);

    /**
     * @brief Returns the wrapped peoplemon
     *
     */
    OwnedPeoplemon& base();

    /**
     * @brief Returns the current stats of the peoplemon, including changes
     *
     */
    const Stats& currentStats() const;

    /**
     * @brief Apply a stage change to the given stat
     *
     * @param stat The stat to change
     * @param diff The number of stages to change by (negative to reduce)
     */
    void statChange(Stat stat, int diff);

    /**
     * @brief Returns whether or not the peoplemon has an ailment of any kind
     *
     */
    bool hasAilment() const;

    /**
     * @brief Gives the peoplemon a passive ailment. Use base() for active ailment
     *
     * @param ail The ailment to give
     * @param sleepTurns Optionally limit turns asleep for the sleep ailment
     */
    void giveAilment(PassiveAilment ail, unsigned int sleepTurns = 1000);

    /**
     * @brief Clear the given passive ailment
     *
     */
    void clearAilment(PassiveAilment ail);

    /**
     * @brief Clears all ailments and optionally the active ailment as well
     *
     */
    void clearAilments(bool includeActive = true);

    /**
     * @brief Returns the current ability of this peoplemon
     *
     */
    SpecialAbility currentAbility() const;

    /**
     * @brief Set an ability override, or None to fallback on the default
     *
     */
    void setCurrentAbility(SpecialAbility ability);

    /**
     * @brief Returns how many turns have passed with an ailment
     *
     */
    unsigned int turnsWithAilment() const;

    /**
     * @brief Returns how many turns this peoplemon has been confused for
     *
     */
    unsigned int turnsConfused();

    /**
     * @brief Returns turns left for Sleep
     *
     */
    unsigned int turnsUntilAwake();

    /**
     * @brief Notify of a turn passing to update internal state
     *
     */
    void notifyTurn();

    /**
     * @brief Notify that a super effective move was just used
     *
     * @param move The super effective move to hit(?) this peoplemon
     */
    void notifySuperEffectiveHit(MoveId move);

private:
    OwnedPeoplemon* ppl;
    Stats cached;
    Stats stages;
    PassiveAilment ailments;
    SpecialAbility ability;
    unsigned int turnsWithAilment;
    unsigned int turnsConfused;
    unsigned int turnsUntilAwake;
    MoveId lastSuperEffectiveTaken;
};

} // namespace pplmn
} // namespace core

#endif
