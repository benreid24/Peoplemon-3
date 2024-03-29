#include <Core/Peoplemon/OwnedPeoplemon.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Debug/DebugOverrides.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <cmath>

namespace core
{
namespace pplmn
{
namespace
{
float statusBonus(Ailment ail) {
    switch (ail) {
    case Ailment::Sleep:
    case Ailment::Frozen:
        return 2.f;
    case Ailment::Annoyed:
    case Ailment::Frustrated:
    case Ailment::Sticky:
        return 1.5f;
    default:
        return 1.f;
    }
}
} // namespace

OwnedPeoplemon::OwnedPeoplemon()
: _id(Id::Unknown)
, level(1)
, xp(0)
, hp(0)
, ailment(Ailment::None)
, item(item::Id::None)
, pendingEvolve(false) {}

OwnedPeoplemon::OwnedPeoplemon(Id ppl, unsigned int l)
: OwnedPeoplemon() {
    _id   = ppl;
    level = l;
    ivs.randomize();
    hp = currentStats().hp;
}

bool OwnedPeoplemon::loadLegacyFile(const std::string& f) {
    bl::serial::binary::InputFile input(
        bl::util::FileUtil::joinPath(Properties::LegacyPeoplemonPath(), f));
    if (!input.good()) return false;

    if (!input.read(customName)) return false;

    std::uint16_t u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    _id = Peoplemon::cast(u16);
    if (_id == Id::Unknown) {
        BL_LOG_ERROR << "Bad peoplemon id " << u16 << " in " << f;
        return false;
    }

    if (!input.read<std::uint16_t>(u16)) return false;
    level = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    xp = u16;
    if (!input.read<std::uint16_t>(u16)) return false; // xp for level up, discard
    if (!input.read<std::uint16_t>(u16)) return false;
    hp = u16;

    std::uint8_t u8;
    if (!input.read<std::uint8_t>(u8)) return false;
    ailment = static_cast<Ailment>(u8);

    if (!input.read<std::uint16_t>(u16)) return false;
    item = item::Item::cast(u16);
    if (item == item::Id::None) {
        BL_LOG_ERROR << "Bad item id " << u16 << " in " << f;
        return false;
    }

    for (int i = 0; i < 4; ++i) {
        if (!input.read<std::uint16_t>(u16)) return false;
        moves[i] = OwnedMove(Move::cast(u16));
        if (!input.read<std::uint16_t>(u16)) return false; // pp, discard
    }

    if (!input.read<std::uint16_t>(u16)) return false;
    ivs.hp = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    ivs.atk = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    ivs.def = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    ivs.spatk = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    ivs.spdef = u16;
    if (!input.read<std::uint16_t>(u16)) return false; // acc
    if (!input.read<std::uint16_t>(u16)) return false; // evade
    if (!input.read<std::uint16_t>(u16)) return false;
    ivs.spd = u16;
    if (!input.read<std::uint16_t>(u16)) return false; // crit

    if (!input.read<std::uint16_t>(u16)) return false;
    evs.hp = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    evs.atk = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    evs.def = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    evs.spatk = u16;
    if (!input.read<std::uint16_t>(u16)) return false;
    evs.spdef = u16;
    if (!input.read<std::uint16_t>(u16)) return false; // acc
    if (!input.read<std::uint16_t>(u16)) return false; // evade
    if (!input.read<std::uint16_t>(u16)) return false;
    evs.spd = u16;
    if (!input.read<std::uint16_t>(u16)) return false; // crit

    return true;
}

Id OwnedPeoplemon::id() const { return _id; }

void OwnedPeoplemon::evolve() {
    const Id eid = evolvesInto();
    if (eid != Id::Unknown) { _id = eid; }
    else { BL_LOG_ERROR << "Tried to evolve " << _id << " but it has no valid evolution!"; }
}

const std::string& OwnedPeoplemon::name() const {
    return customName.empty() ? Peoplemon::name(_id) : customName;
}

Type OwnedPeoplemon::type() const { return Peoplemon::type(_id); }

void OwnedPeoplemon::setCustomName(const std::string& n) { customName = n; }

unsigned int OwnedPeoplemon::currentLevel() const { return level; }

unsigned int OwnedPeoplemon::evolveLevel() const { return Peoplemon::evolveLevel(_id); }

Id OwnedPeoplemon::evolvesInto() const { return Peoplemon::evolvesInto(_id); }

bool OwnedPeoplemon::pendingEvolution() const { return pendingEvolve; }

bool& OwnedPeoplemon::pendingEvolution() { return pendingEvolve; }

unsigned int OwnedPeoplemon::currentXP() const { return xp; }

unsigned int OwnedPeoplemon::nextLevelXP() const { return Peoplemon::levelUpXp(_id, level); }

unsigned int OwnedPeoplemon::awardXP(unsigned int award) {
    if (level >= 100) {
        xp = 0;
        return 0;
    }

    const unsigned int req = Peoplemon::levelUpXp(_id, level);
    if (xp + award >= req) {
        const unsigned int aa = req - xp;
        xp                    = req;
        return award - aa;
    }
    else {
        xp += award;
        return 0;
    }
}

Stats OwnedPeoplemon::currentStats() const {
    return Stats::computeStats(Peoplemon::baseStats(_id), evs, ivs, level);
}

const Stats& OwnedPeoplemon::currentEVs() const { return evs; }

const Stats& OwnedPeoplemon::currentIVs() const { return ivs; }

std::uint16_t& OwnedPeoplemon::currentHp() { return hp; }

void OwnedPeoplemon::applyDamage(int dmg) {
    std::uint16_t udmg = dmg;
    udmg               = std::min(udmg, hp);
    hp -= udmg;
}

void OwnedPeoplemon::giveHealth(int p) { hp = std::min(hp + p, currentStats().hp); }

std::uint16_t OwnedPeoplemon::currentHp() const { return hp; }

void OwnedPeoplemon::awardEVs(const Stats& award) { evs.addEVs(award); }

Ailment& OwnedPeoplemon::currentAilment() { return ailment; }

item::Id& OwnedPeoplemon::holdItem() { return item; }

Ailment OwnedPeoplemon::currentAilment() const { return ailment; }

item::Id OwnedPeoplemon::holdItem() const { return item; }

SpecialAbility OwnedPeoplemon::ability() const { return Peoplemon::specialAbility(_id); }

bool OwnedPeoplemon::knowsMove(MoveId m) const {
    for (unsigned int i = 0; i < 4; ++i) {
        if (moves[i].id == m) return true;
    }
    return false;
}

const OwnedMove* OwnedPeoplemon::knownMoves() const { return moves; }

OwnedMove* OwnedPeoplemon::knownMoves() { return moves; }

void OwnedPeoplemon::learnMove(MoveId m, unsigned int i) { moves[i] = OwnedMove(m); }

bool OwnedPeoplemon::gainMove(MoveId m) {
    for (int i = 0; i < 4; ++i) {
        if (moves[i].id == MoveId::Unknown) {
            moves[i] = OwnedMove(m);
            return true;
        }
    }
    return false;
}

void OwnedPeoplemon::heal() {
    ailment = Ailment::None;
    hp      = currentStats().hp;
}

bool OwnedPeoplemon::hasExpShare() const {
    // TODO - check hold item
    return false;
}

unsigned int OwnedPeoplemon::xpYield(bool trainer) const {
    unsigned int xp = level;
    if (trainer) { xp = xp * 3 / 2; }
    xp *= Peoplemon::xpYieldMultiplier(_id);
    return xp / 7;
}

MoveId OwnedPeoplemon::levelUp() {
    if (level < 100) {
        level += 1;
        xp = 0;
        return Peoplemon::moveLearnedAtLevel(_id, level);
    }
    return MoveId::Unknown;
}

OwnedMove* OwnedPeoplemon::findMove(MoveId id) {
    for (int i = 0; i < 4; ++i) {
        if (moves[i].id == id) return &moves[i];
    }
    return nullptr;
}

bool OwnedPeoplemon::shakePasses(item::Id ball, int turnNumber, unsigned int opLevel) {
    // https://bulbapedia.bulbagarden.net/wiki/Catch_rate#Capture_method_.28Generation_III-IV.29

#ifdef PEOPLEMON_DEBUG
    if (debug::DebugOverrides::get().alwaysCatch) return true;
#endif

    const float chp2       = static_cast<float>(currentHp()) * 2.f;
    const float mhp3       = static_cast<float>(currentStats().hp) * 3.f;
    const float levelRatio = static_cast<float>(opLevel) / static_cast<float>(currentLevel());
    const float br         = item::Item::getPeopleballRate(ball, _id, turnNumber, levelRatio);
    const float ar         = statusBonus(currentAilment());
    const float r          = Peoplemon::catchRate(_id);

    const int a = (mhp3 - chp2) * r * br / mhp3 * ar;
    const int b = 1048560 / static_cast<int>(std::sqrt(static_cast<int>(std::sqrt(16711680 / a))));
    const bool passed = a < 255 ? bl::util::Random::get<int>(0, 65535) < b : true;
    BL_LOG_INFO << "ModifiedCatchRate=" << a << " | ShakeProbability=" << b << " | "
                << (passed ? "Passed" : "Failed");
    return passed;
}

bool OwnedPeoplemon::canClone() const { return Peoplemon::canClone(_id); }

} // namespace pplmn
} // namespace core
