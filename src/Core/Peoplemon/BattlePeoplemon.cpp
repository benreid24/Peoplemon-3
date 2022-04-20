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
, _turnsWithAilment(0)
, _turnsConfused(0)
, _turnsUntilAwake(0)
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

void BattlePeoplemon::statChange(Stat stat, int diff) {
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
    *val += diff;
    if (*val > 6) *val = 6;
    if (*val < -6) *val = -6;
    refreshStats();
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

bool BattlePeoplemon::giveAilment(Ailment a, std::uint16_t st) {
    if (ppl->currentAilment() != Ailment::None) return false;
    ppl->currentAilment() = a;
    _turnsUntilAwake      = st;
    return true;
}

void BattlePeoplemon::clearAilment(PassiveAilment ail) {
    using T     = std::underlying_type_t<PassiveAilment>;
    const T neg = ~static_cast<T>(ail);
    ailments    = static_cast<PassiveAilment>(static_cast<T>(ailments) & neg);
}

void BattlePeoplemon::clearAilments(bool a) {
    ailments = PassiveAilment::None;
    if (a) ppl->currentAilment() = Ailment::None;
}

SpecialAbility BattlePeoplemon::currentAbility() const { return ability; }

void BattlePeoplemon::setCurrentAbility(SpecialAbility a) { ability = a; }

std::uint16_t BattlePeoplemon::turnsWithAilment() const { return _turnsWithAilment; }

std::uint16_t BattlePeoplemon::turnsConfused() const { return _turnsConfused; }

std::uint16_t BattlePeoplemon::turnsUntilAwake() const { return _turnsUntilAwake; }

void BattlePeoplemon::notifyTurn() {
    if (hasAilment()) { ++_turnsWithAilment; }
    else {
        _turnsWithAilment = 0;
    }

    if (hasAilment(PassiveAilment::Confused)) { ++_turnsConfused; }
    else {
        _turnsConfused = 0;
    }

    if (_turnsUntilAwake > 0) { --_turnsUntilAwake; }
}

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

} // namespace pplmn
} // namespace core
