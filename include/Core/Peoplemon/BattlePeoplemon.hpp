#ifndef CORE_PEOPLEMON_BATTLEPEOPLEMON_HPP
#define CORE_PEOPLEMON_BATTLEPEOPLEMON_HPP

#include <Core/Peoplemon/BattleStats.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Peoplemon/PassiveAilment.hpp>
#include <Core/Peoplemon/SpecialAbility.hpp>

namespace core
{
namespace battle
{
struct BattlerSubstate;
}
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
    const BattleStats& battleStages() const;

    /**
     * @brief Returns the speed of this peoplemon, adjusted for ailments
     *
     */
    int getSpeed() const;

    /**
     * @brief Applies damage to the peoplemon. Ensures that the hp does not go negative
     *
     * @param dmg The amount of hp to reduce
     */
    void applyDamage(int dmg);

    /**
     * @brief Restores HP and ensures it does not go over max HP
     *
     * @param hp The amount of hp to restore
     */
    void giveHealth(int hp);

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
     * @param state The state of the battler the peoplemon belongs to
     *
     */
    bool hasAilment(const battle::BattlerSubstate& state) const;

    /**
     * @brief Returns true if the peoplemon has the specific ailment
     *
     * @param ail The ailment to check for
     * @return True if the peoplemon has it, false otherwise
     */
    bool hasAilment(Ailment ail) const;

    /**
     * @brief Gives the peoplemon an ailment
     *
     * @param ail The ailment to give
     * @return True if the ailment was given, false if an ailment is already present
     */
    bool giveAilment(Ailment ail);

    /**
     * @brief Clears all ailments the peoplemon has, including passive ailments if passed in
     *
     * @param state Optional battler state
     * @return True if any ailments were cleared, false if already healthy
     */
    bool clearAilments(battle::BattlerSubstate* state);

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

    /**
     * @brief The number of turns until the peoplemon wakes up
     *
     * @return std::int8_t&
     */
    std::int8_t& turnsUntilAwake();

private:
    OwnedPeoplemon* ppl;
    Stats cached;
    Stats stages;
    BattleStats stageOnlys;
    SpecialAbility ability;
    bool sawBattle;
    std::int8_t _turnsUntilAwake;

    void refreshStats();
};

} // namespace pplmn
} // namespace core

#endif
