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
     * @return True if the stat could be changed, false if not
     */
    bool statChange(Stat stat, int diff);

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
     * @return True if the ailment was given, false if an ailment is already present
     */
    bool giveAilment(Ailment ail);

    /**
     * @brief Clear the given passive ailment
     *
     */
    void clearAilment(PassiveAilment ail);

    /**
     * @brief Clears all ailments and optionally the active ailment as well
     *
     * @param includeActive True to clear the active ailment as well, false for passive only
     * @return True if any ailments were cleared, false if not
     *
     */
    bool clearAilments(bool includeActive);

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

    /**
     * @brief Copies the stat stages from the other peoplemon
     *
     * @param other The peoplemon to copy from
     */
    void copyStages(const BattlePeoplemon& other);

    /**
     * @brief Resets all stat stages
     *
     */
    void resetStages();

    /**
     * @brief Lets the peoplemon know that they have seen battle
     *
     */
    void notifyInBattle();

    /**
     * @brief Resets that this peoplemon has seen battle
     *
     */
    void resetSawBattle();

    /**
     * @brief Returns whether or not this peoplemon has seen battle (for exp gain)
     *
     */
    bool hasSeenBattle() const;

private:
    OwnedPeoplemon* ppl;
    Stats cached;
    Stats stages;
    BattleStats cachedBattle;
    BattleStats battleStages;
    PassiveAilment ailments;
    SpecialAbility ability;
    MoveId lastSuperEffectiveTaken;
    bool sawBattle;

    void refreshStats();
};

} // namespace pplmn
} // namespace core

#endif
