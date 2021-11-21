#ifndef CORE_PEOPLEMON_STATS_HPP
#define CORE_PEOPLEMON_STATS_HPP

#include <Core/Peoplemon/Stat.hpp>
#include <array>

namespace core
{
namespace pplmn
{
/**
 * @brief Stats for Peoplemon. This struct is used for base stats, EVs, IVs, battle
 *        increases/decreases, and base stats
 *
 * @ingroup Peoplemon
 *
 */
struct Stats {
    int hp;
    int atk;
    int def;
    int spatk;
    int spdef;
    int acc;
    int evade;
    int spd;
    int crit;

    /**
     * @brief Initializes all members to 0
     *
     */
    Stats();

    /**
     * @brief Returns the sum of the 6 stats that are used for EV calculations
     *
     * @return int The sum of each stat
     */
    int sum() const;

    /**
     * @brief Generates random IV stats
     *
     */
    void randomize();

    /**
     * @brief Returns a reference to the given stat
     *
     */
    int& get(Stat stat);

    /**
     * @brief Returns the given stat
     *
     */
    int get(Stat stat) const;

    /**
     * @brief Helper method to compute a Peoplemon's current stats
     *
     * @param base Base stats for the Peoplemon to compute
     * @param evs The EVs of the Peoplemon
     * @param ivs The IVs of the Peoplemon
     * @param level The current level of the Peoplemon
     * @param stages Stat increases/decreases during battle
     * @return Stats The Peoplemon's current stats
     */
    static Stats computeStats(const Stats& base, const Stats& evs, const Stats& ivs,
                              unsigned int level, const Stats& stages = {});

    /// Helper array to iterate over stats in loop
    static const std::array<Stat, 9> IterableStats;

    /// The maximum amount that EVs or IVs can sum to
    static constexpr int MaxEVSum = 510;

    /// The maximum amount that a single EV can be
    static constexpr int MaxEVStat = 255;

    /// The maximum amount that a single IV can be
    static constexpr int MaxIVStat = 31;
};

} // namespace pplmn
} // namespace core

#endif
