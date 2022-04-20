#ifndef CORE_PEOPLEMON_BATTLEPEOPLEMON_HPP
#define CORE_PEOPLEMON_BATTLEPEOPLEMON_HPP

#include <Core/Peoplemon/BattleStats.hpp>
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
     * @brief Returns the wrapped peoplemon
     *
     */
    const OwnedPeoplemon& base() const;

    /**
     * @brief Returns the current stats of the peoplemon, including changes
     *
     */
    const Stats& currentStats() const;

    /**
     * @brief Returns the current battle-only stats of the peoplemon, including changes
     *
     */
    const BattleStats& battleStats() const;

    /**
     * @brief Applies damage to the peoplemon. Ensures that the hp does not go negative
     *
     * @param dmg The amount of hp to reduce
     */
    void applyDamage(int dmg);

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
     * @brief Returns true if the peoplemon has the specific ailment
     *
     * @param ail The ailment to check for
     * @return True if the peoplemon has it, false otherwise
     */
    bool hasAilment(PassiveAilment ail) const;

    /**
     * @brief Returns true if the peoplemon has the specific ailment
     *
     * @param ail The ailment to check for
     * @return True if the peoplemon has it, false otherwise
     */
    bool hasAilment(Ailment ail) const;

    /**
     * @brief Gives the peoplemon a passive ailment
     *
     * @param ail The ailment to give
     */
    void giveAilment(PassiveAilment ail);

    /**
     * @brief Gives the peoplemon an ailment
     *
     * @param ail The ailment to give
     * @param sleepTurns Optionally limit turns asleep for the sleep ailment
     * @return True if the ailment was given, false if an ailment is already present
     */
    bool giveAilment(Ailment ail, std::uint16_t sleepTurns = 1000);

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
    std::uint16_t turnsWithAilment() const;

    /**
     * @brief Returns how many turns this peoplemon has been confused for
     *
     */
    std::uint16_t turnsConfused() const;

    /**
     * @brief Returns turns left for Sleep
     *
     */
    std::uint16_t turnsUntilAwake() const;

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

    /**
     * @brief Returns the id of the last move used against this peoplemon that was super effective
     *
     */
    MoveId mostRecentSuperEffectiveHit() const;

private:
    OwnedPeoplemon* ppl;
    Stats cached;
    Stats stages;
    BattleStats cachedBattle;
    BattleStats battleStages;
    PassiveAilment ailments;
    SpecialAbility ability;
    std::uint16_t _turnsWithAilment;
    std::uint16_t _turnsConfused;
    std::uint16_t _turnsUntilAwake;
    MoveId lastSuperEffectiveTaken;

    void refreshStats();
};

} // namespace pplmn
} // namespace core

#endif
