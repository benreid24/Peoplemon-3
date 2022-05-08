#include <Core/Peoplemon/BattlePeoplemon.hpp>

#include <Core/Battles/BattlerSubstate.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace core
{
namespace pplmn
{
BattlePeoplemon::BattlePeoplemon(OwnedPeoplemon* p)
: ppl(p)
, cached(p->currentStats())
, stageOnlys(true)
, ability(Peoplemon::specialAbility(p->id()))
, lastSuperEffectiveTaken(MoveId::Unknown)
, sawBattle(false) {}

OwnedPeoplemon& BattlePeoplemon::base() { return *ppl; }

const OwnedPeoplemon& BattlePeoplemon::base() const { return *ppl; }

const Stats& BattlePeoplemon::currentStats() const { return cached; }

const BattleStats& BattlePeoplemon::battleStages() const { return stageOnlys; }

void BattlePeoplemon::applyDamage(int dmg) {
    std::uint16_t udmg = dmg;
    udmg               = std::min(udmg, ppl->hp);
    ppl->hp -= udmg;
}

bool BattlePeoplemon::statChange(Stat stat, int diff) {
    int* val = nullptr;
    switch (stat) {
    case Stat::Evasion:
        val = &stageOnlys.evade;
        break;
    case Stat::Accuracy:
        val = &stageOnlys.acc;
        break;
    case Stat::Critical:
        val = &stageOnlys.crit;
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

bool BattlePeoplemon::hasAilment(const battle::BattlerSubstate& state) const {
    return ppl->currentAilment() != Ailment::None || state.ailments != PassiveAilment::None;
}

bool BattlePeoplemon::hasAilment(Ailment ail) const { return ppl->currentAilment() == ail; }

bool BattlePeoplemon::giveAilment(Ailment a) {
    if (ppl->currentAilment() != Ailment::None) return false;
    ppl->currentAilment() = a;
    return true;
}

bool BattlePeoplemon::clearAilments(battle::BattlerSubstate* state) {
    bool ret = false;
    if (state && state->ailments != PassiveAilment::None) {
        state->ailments = PassiveAilment::None;
    }
    if (ppl->currentAilment() != Ailment::None) {
        ppl->currentAilment() = Ailment::None;
        ret                   = true;
    }
    return ret;
}

SpecialAbility BattlePeoplemon::currentAbility() const { return ability; }

void BattlePeoplemon::setCurrentAbility(SpecialAbility a) { ability = a; }

void BattlePeoplemon::notifySuperEffectiveHit(MoveId m) { lastSuperEffectiveTaken = m; }

MoveId BattlePeoplemon::mostRecentSuperEffectiveHit() const { return lastSuperEffectiveTaken; }

void BattlePeoplemon::refreshStats() {
    cached = Stats::computeStats(Peoplemon::baseStats(ppl->id()),
                                 ppl->currentEVs(),
                                 ppl->currentIVs(),
                                 ppl->currentLevel(),
                                 stages);
}

void BattlePeoplemon::copyStages(const BattlePeoplemon& o) {
    stages     = o.stages;
    stageOnlys = o.stageOnlys;
    refreshStats();
}

void BattlePeoplemon::resetStages() {
    stages.atk       = 0;
    stages.def       = 0;
    stages.hp        = 0;
    stages.spatk     = 0;
    stages.spdef     = 0;
    stages.spd       = 0;
    stageOnlys.acc   = 0;
    stageOnlys.crit  = 0;
    stageOnlys.evade = 0;
    refreshStats();
}

void BattlePeoplemon::notifyInBattle() { sawBattle = true; }

void BattlePeoplemon::resetSawBattle() { sawBattle = false; }

bool BattlePeoplemon::hasSeenBattle() const { return sawBattle || base().hasExpShare(); }

} // namespace pplmn
} // namespace core
