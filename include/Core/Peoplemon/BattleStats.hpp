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
     * @brief Computes the actual stat values
     *
     * @param stats The base battle stats
     * @param stages Any stat changes that have been made
     * @return BattleStats The actual stats to use in battle
     */
    static BattleStats computeStats(const BattleStats& stats, const BattleStats& stages);
};

} // namespace pplmn
} // namespace core

#endif
