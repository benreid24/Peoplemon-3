#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/Stats.hpp>

namespace core
{
namespace pplmn
{
const std::array<Stat, 9> Stats::IterableStats = {Stat::HP,
                                                  Stat::Attack,
                                                  Stat::Defense,
                                                  Stat::SpecialAttack,
                                                  Stat::SpecialDefense,
                                                  Stat::Accuracy,
                                                  Stat::Evasion,
                                                  Stat::Speed,
                                                  Stat::Critical};

Stats::Stats()
: hp(0)
, atk(0)
, def(0)
, spatk(0)
, spdef(0)
, acc(0)
, evade(0)
, spd(0)
, crit(0) {}

int Stats::sum() const { return hp + atk + def + spd + spatk + spdef; }

void Stats::randomize() {
    hp    = bl::util::Random::get<int>(0, MaxIVStat);
    atk   = bl::util::Random::get<int>(0, MaxIVStat);
    def   = bl::util::Random::get<int>(0, MaxIVStat);
    spatk = bl::util::Random::get<int>(0, MaxIVStat);
    spdef = bl::util::Random::get<int>(0, MaxIVStat);
    spd   = bl::util::Random::get<int>(0, MaxIVStat);
}

int& Stats::get(Stat s) {
    const unsigned int i = static_cast<unsigned int>(s);
    return (&hp)[i];
}

int Stats::get(Stat s) const {
    const unsigned int i = static_cast<unsigned int>(s);
    return (&hp)[i];
}

} // namespace pplmn
} // namespace core
