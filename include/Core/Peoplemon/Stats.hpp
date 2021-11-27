#ifndef CORE_PEOPLEMON_STATS_HPP
#define CORE_PEOPLEMON_STATS_HPP

#include <BLIB/Files/Binary/Serializer.hpp>
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
    int spd;

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
     * @brief Returns the multiplier for a given stat increase/decrease
     *
     * @param stage The current stat stage in range [-6, 6]
     * @return int The multiplier to apply
     */
    static int stageMultiplier(int stage);

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

    /**
     * @brief Converts the given stat to its string representation
     *
     * @param stat The stat to convert
     * @return const char* The string representation of the stat
     */
    static const char* statToString(Stat stat);

    /// Helper array to iterate over stats in loop
    static const std::array<Stat, 6> IterableStats;

    /// The maximum amount that EVs or IVs can sum to
    static constexpr int MaxEVSum = 510;

    /// The maximum amount that a single EV can be
    static constexpr int MaxEVStat = 255;

    /// The maximum amount that a single IV can be
    static constexpr int MaxIVStat = 31;
};

} // namespace pplmn
} // namespace core

namespace bl
{
namespace file
{
namespace binary
{
using core::pplmn::Stats;

template<>
struct Serializer<Stats> {
    static bool serialize(File& output, const Stats& v) {
        if (!output.write<std::uint16_t>(v.hp)) return false;
        if (!output.write<std::uint16_t>(v.atk)) return false;
        if (!output.write<std::uint16_t>(v.def)) return false;
        if (!output.write<std::uint16_t>(v.spatk)) return false;
        if (!output.write<std::uint16_t>(v.spdef)) return false;
        if (!output.write<std::uint16_t>(v.spd)) return false;
        return true;
    }

    static bool deserialize(File& input, Stats& v) {
        std::uint16_t val = 0;
        if (!input.read<std::uint16_t>(val)) return false;
        v.hp = val;
        if (!input.read<std::uint16_t>(val)) return false;
        v.atk = val;
        if (!input.read<std::uint16_t>(val)) return false;
        v.def = val;
        if (!input.read<std::uint16_t>(val)) return false;
        v.spatk = val;
        if (!input.read<std::uint16_t>(val)) return false;
        v.spdef = val;
        if (!input.read<std::uint16_t>(val)) return false;
        v.spd = val;
        return true;
    }

    static std::size_t size(const Stats&) { return sizeof(std::uint16_t) * 6; }
};

} // namespace binary
} // namespace file
} // namespace bl

#endif
