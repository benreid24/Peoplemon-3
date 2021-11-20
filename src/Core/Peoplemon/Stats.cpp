#include <Core/Peoplemon/Stats.hpp>

namespace core
{
namespace pplmn
{
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
