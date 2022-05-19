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

Stats::Stats(Stat stat, int p)
: Stats() {
    get(stat) = p;
}

int Stats::sum() const { return hp + atk + def + spd + spatk + spdef; }

void Stats::randomize() {
    for (const Stat stat : IterableStats) { get(stat) = bl::util::Random::get<int>(0, MaxIVStat); }
}

void Stats::addEVs(const Stats& award) {
    for (const Stat stat : Stats::IterableStats) {
        int a = std::min(award.get(stat), Stats::MaxEVSum - sum());
        a     = std::min(a, Stats::MaxEVStat - get(stat));
        get(stat) += a;
    }
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
        const float sm   = stageMultiplier(stages.get(stat));
        const int inside = ((base * 2 + iv + ev / 4) * level) / 100;
        if (stat == Stat::HP) { result.get(stat) = inside + level + 10; }
        else {
            result.get(stat) = static_cast<float>(inside + 5) * sm;
        }
    }
    return result;
}

float Stats::stageMultiplier(int stage) {
    const float s = static_cast<float>(stage);
    if (stage <= 0) { return 2.f / (2.f - static_cast<float>(s)); }
    else {
        return (2.f + static_cast<float>(s)) / 2.f;
    }
}

const char* Stats::statToString(Stat s) {
    switch (s) {
    case Stat::Attack:
        return "ATK";
    case Stat::Defense:
        return "DEF";
    case Stat::HP:
        return "HP";
    case Stat::SpecialAttack:
        return "SPATK";
    case Stat::SpecialDefense:
        return "SPDEF";
    case Stat::Speed:
        return "SPD";
    case Stat::Accuracy:
        return "ACC";
    case Stat::Critical:
        return "CRIT";
    case Stat::Evasion:
        return "EVD";
    default:
        return "<ERR>";
    }
}

} // namespace pplmn
} // namespace core
