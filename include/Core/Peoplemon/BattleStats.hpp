#ifndef CORE_PEOPLEMON_BATTLESTATS_HPP
#define CORE_PEOPLEMON_BATTLESTATS_HPP

#include <Core/Peoplemon/Stats.hpp>

namespace core
{
namespace pplmn
{
/**
 * @brief Stores stats that only exist during battle
 *
 * @ingroup Peoplemon
 *
 */
struct BattleStats {
    int acc;
    int evade;
    int crit;

    /**
     * @brief Initializes all stats to 0 or to their defaults
     *
     * @param zero True to zero, false for defaults
     *
     */
    BattleStats(bool zero);

    /**
     * @brief Returns the accuracy multiplier for a move hitting
     *
     * @param adjustedStage Users acc stage minus defenders evd stage, clamped to [-6, 6]
     * @return float The multiplier to use
     */
    static float getAccuracyMultiplier(int adjustedStage);
};

} // namespace pplmn
} // namespace core

#endif
