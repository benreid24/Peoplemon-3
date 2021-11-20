#ifndef CORE_PEOPLEMON_STATS_HPP
#define CORE_PEOPLEMON_STATS_HPP

#include <Core/Peoplemon/Stat.hpp>

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
     * @brief Returns a reference to the given stat
     *
     */
    int& get(Stat stat);

    /**
     * @brief Returns the given stat
     *
     */
    int get(Stat stat) const;

    // TODO - helper methods for combining stages etc
};

} // namespace pplmn
} // namespace core

#endif
