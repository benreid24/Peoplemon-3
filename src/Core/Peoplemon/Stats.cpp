#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/BattleStats.hpp>
#include <Core/Peoplemon/Stats.hpp>

namespace core
{
namespace pplmn
{
const std::array<Stat, 6> Stats::IterableStats = {
    Stat::HP, Stat::Attack, Stat::Defense, Stat::SpecialAttack, Stat::SpecialDefense, Stat::Speed};

Stats::Stats()
: hp(0)
, atk(0)
, def(0)
, spatk(0)
, spdef(0)
, spd(0) {}

int Stats::sum() const { return hp + atk + def + spd + spatk + spdef; }

void Stats::randomize() {
    for (const Stat stat : IterableStats) { get(stat) = bl::util::Random::get<int>(0, MaxIVStat); }
}

int& Stats::get(Stat s) {
    const unsigned int i = static_cast<unsigned int>(s);
    return (&hp)[i];
}

int Stats::get(Stat s) const {
    const unsigned int i = static_cast<unsigned int>(s);
    return (&hp)[i];
}

Stats Stats::computeStats(const Stats& bases, const Stats& evs, const Stats& ivs,
                          unsigned int level, const Stats& stages) {
    Stats result;
    for (const Stat stat : IterableStats) {
        const int base   = bases.get(stat);
        const int ev     = evs.get(stat);
        const int iv     = ivs.get(stat);
        const int sm     = stageMultiplier(stages.get(stat));
        result.get(stat) = ((base * 2 + iv + ev / 4) * (level / 100) + 5) * sm;
    }
    return result;
}

int Stats::stageMultiplier(int stage) {
    const float s = static_cast<float>(stage);
    if (stage <= 0) { return 2.f / (2.f - s); }
    else {
        return (2.f + s) / 2.f;
    }
}

} // namespace pplmn
} // namespace core
