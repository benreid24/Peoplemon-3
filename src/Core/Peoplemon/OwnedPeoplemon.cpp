#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace core
{
namespace pplmn
{
OwnedPeoplemon::OwnedPeoplemon()
: id(Id::Unknown)
, level(1)
, xp(0)
, hp(0)
, ailment(Ailment::None)
, item(item::Id::None) {}

OwnedPeoplemon::OwnedPeoplemon(Id ppl, unsigned int l)
: OwnedPeoplemon() {
    id    = ppl;
    level = l;
    hp    = currentStats().hp;
    ivs.randomize();
}

const std::string& OwnedPeoplemon::name() const {
    return customName.empty() ? Peoplemon::name(id) : customName;
}

void OwnedPeoplemon::setCustomName(const std::string& n) { customName = n; }

unsigned int OwnedPeoplemon::currentLevel() const { return level; }

unsigned int OwnedPeoplemon::currentXP() const { return xp; }

unsigned int OwnedPeoplemon::awardXP(unsigned int award) {
    if (level >= 100) {
        xp = 0;
        return 0;
    }

    const unsigned int req = Peoplemon::levelUpXp(id, level);
    if (xp + award >= req) {
        const unsigned int aa = req - xp;
        xp                    = 0;
        level += 1;
        return award - aa;
    }
    else {
        xp += award;
        return 0;
    }
}

Stats OwnedPeoplemon::currentStats() const {
    return Stats::computeStats(Peoplemon::baseStats(id), evs, ivs, level);
}

unsigned int& OwnedPeoplemon::currentHp() { return hp; }

void OwnedPeoplemon::awardEVs(const Stats& award) {
    for (const Stat stat : Stats::IterableStats) {
        int a = std::min(award.get(stat), Stats::MaxEVSum - evs.sum());
        a     = std::min(a, Stats::MaxEVStat - evs.get(stat));
        evs.get(stat) += a;
    }
}

Ailment& OwnedPeoplemon::currentAilment() { return ailment; }

item::Id& OwnedPeoplemon::holdItem() { return item; }

} // namespace pplmn
} // namespace core
