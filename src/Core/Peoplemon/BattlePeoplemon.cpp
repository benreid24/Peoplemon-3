#include <Core/Peoplemon/BattlePeoplemon.hpp>

#include <Core/Peoplemon/Peoplemon.hpp>

namespace core
{
namespace pplmn
{
BattlePeoplemon::BattlePeoplemon(OwnedPeoplemon* p)
: ppl(p)
, cached(p->currentStats())
, cachedBattle(false)
, battleStages(true)
, ailments(PassiveAilment::None)
, ability(Peoplemon::specialAbility(p->id()))
, lastSuperEffectiveTaken(MoveId::Unknown) {}

OwnedPeoplemon& BattlePeoplemon::base() { return *ppl; }

const OwnedPeoplemon& BattlePeoplemon::base() const { return *ppl; }

const Stats& BattlePeoplemon::currentStats() const { return cached; }

const BattleStats& BattlePeoplemon::battleStats() const { return cachedBattle; }

void BattlePeoplemon::applyDamage(int dmg) {
    std::uint16_t udmg = dmg;
    udmg               = std::min(udmg, ppl->hp);
    ppl->hp -= udmg;
}

bool BattlePeoplemon::statChange(Stat stat, int diff) {
    int* val = nullptr;
    switch (stat) {
    case Stat::Evasion:
        val = &battleStages.evade;
        break;
    case Stat::Accuracy:
        val = &battleStages.acc;
        break;
    case Stat::Critical:
        val = &battleStages.crit;
        break;
    default:
        val = &stages.get(stat);
        break;
    }
    if (*val == -6 || *val == 6) return false;
    *val += diff;
    if (*val > 6) *val = 6;
    if (*val < -6) *val = -6;
    refreshStats();
    return true;
}

bool BattlePeoplemon::hasAilment() const {
    return ppl->currentAilment() != Ailment::None || ailments != PassiveAilment::None;
}

bool BattlePeoplemon::hasAilment(PassiveAilment ail) const {
    using T = std::underlying_type_t<PassiveAilment>;
    return (static_cast<T>(ailments) & static_cast<T>(ail)) != 0;
}

bool BattlePeoplemon::hasAilment(Ailment ail) const { return ppl->currentAilment() == ail; }

void BattlePeoplemon::giveAilment(PassiveAilment ail) {
    using T  = std::underlying_type_t<PassiveAilment>;
    ailments = static_cast<PassiveAilment>(static_cast<T>(ail) | static_cast<T>(ailments));
}

bool BattlePeoplemon::giveAilment(Ailment a) {
    if (ppl->currentAilment() != Ailment::None) return false;
    ppl->currentAilment() = a;
    return true;
}

void BattlePeoplemon::clearAilment(PassiveAilment ail) {
    using T     = std::underlying_type_t<PassiveAilment>;
    const T neg = ~static_cast<T>(ail);
    ailments    = static_cast<PassiveAilment>(static_cast<T>(ailments) & neg);
}

bool BattlePeoplemon::clearAilments(bool a) {
    bool ret = ailments != PassiveAilment::None;
    ailments = PassiveAilment::None;
    if (a) {
        if (ppl->currentAilment() != Ailment::None) {
            ppl->currentAilment() = Ailment::None;
            ret                   = true;
        }
    }
    return ret;
}

SpecialAbility BattlePeoplemon::currentAbility() const { return ability; }

void BattlePeoplemon::setCurrentAbility(SpecialAbility a) { ability = a; }

void BattlePeoplemon::notifySuperEffectiveHit(MoveId m) { lastSuperEffectiveTaken = m; }

MoveId BattlePeoplemon::mostRecentSuperEffectiveHit() const { return lastSuperEffectiveTaken; }

void BattlePeoplemon::refreshStats() {
    cachedBattle = BattleStats::computeStats({false}, battleStages);
    cached       = Stats::computeStats(Peoplemon::baseStats(ppl->id()),
                                 ppl->currentEVs(),
                                 ppl->currentIVs(),
                                 ppl->currentLevel(),
                                 stages);
}

void BattlePeoplemon::copyStages(const BattlePeoplemon& o) {
    stages       = o.stages;
    battleStages = o.battleStages;
    refreshStats();
}

void BattlePeoplemon::resetStages() {
    stages.atk         = 0;
    stages.def         = 0;
    stages.hp          = 0;
    stages.spatk       = 0;
    stages.spdef       = 0;
    stages.spd         = 0;
    battleStages.acc   = 0;
    battleStages.crit  = 0;
    battleStages.evade = 0;
    refreshStats();
}

} // namespace pplmn
} // namespace core
