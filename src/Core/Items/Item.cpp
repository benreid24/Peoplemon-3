#include <Core/Items/Item.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Properties.hpp>
#include <unordered_map>

namespace core
{
namespace item
{
namespace
{
std::vector<Id> ids;
const std::string UnknownName        = "<Unknown Item>";
const std::string UnknownDescription = "<This item is not known to Peoplemon>";

} // namespace

std::unordered_map<Id, std::string>* Item::names        = nullptr;
std::unordered_map<Id, std::string>* Item::descriptions = nullptr;
std::unordered_map<Id, int>* Item::values               = nullptr;

Id Item::cast(unsigned int id) {
    const Id result = static_cast<Id>(id);

    if (names->find(result) != names->end()) { return result; }
    else {
        BL_LOG_WARN << "Unknown item id: " << id;
        return Id::Unknown;
    }
}

void Item::setDataSource(file::ItemDB& db) {
    names        = &db.names;
    descriptions = &db.descriptions;
    values       = &db.values;

    ids.reserve(names->size());
    for (const auto& p : *names) { ids.emplace_back(p.first); }
    std::sort(ids.begin(), ids.end());
}

Category Item::getCategory(Id item) {
    const unsigned int id = static_cast<unsigned int>(item);
    if (item == Id::Unknown) return Category::Unknown;
    if (id <= 100) return Category::Regular;
    if (id <= 200) return Category::Key;
    return Category::TM;
}

Type Item::getType(Id item) {
    const unsigned int id = static_cast<unsigned int>(item);
    if (id >= 1 && id <= 4) return Type::TargetPeoplemon;
    if (id >= 5 && id <= 17) return Type::Peopleball;
    if (id >= 18 && id <= 27) return Type::TargetPeoplemon;
    if (id >= 28 && id <= 32) return Type::EvolveStone;
    if (id == 33 || id == 34 || id == 44) return Type::Useless;
    if (id >= 35 && id <= 37) return Type::PlayerModifier;
    if (id >= 38 && id <= 43) return Type::TargetPeoplemon;
    if (id >= 50 && id <= 62) return Type::HoldItem;
    if (id >= 101 && id <= 138) return Type::KeyItem;
    if (id >= 200) return Type::TM;
    return Type::Unknown;
}

const std::string& Item::getName(Id item) {
    const auto it = names->find(item);
    if (it == names->end()) {
        const pplmn::MoveId mid = getTmMove(item);
        if (mid != pplmn::MoveId::Unknown) {
            static std::string tmName;
            tmName = "TM " + std::to_string(static_cast<int>(mid));
            return tmName;
        }
        return UnknownName;
    }
    return it->second;
}

const std::string& Item::getDescription(Id item) {
    const auto it = descriptions->find(item);
    if (it == descriptions->end()) {
        const pplmn::MoveId mid = getTmMove(item);
        if (mid != pplmn::MoveId::Unknown) {
            static std::string tmDesc;
            tmDesc = "This TM teaches " + pplmn::Move::name(mid) + " to a Peoplemon";
            return tmDesc;
        }
        return UnknownDescription;
    }
    return it->second;
}

int Item::getValue(Id item) {
    const auto it = values->find(item);
    if (it == values->end()) { return getCategory(item) == Category::TM ? 2500 : 0; }
    return it->second;
}

const std::vector<Id>& Item::validIds() { return ids; }

bool Item::canUseInBattle(Id id) {
    const Type t = getType(id);
    switch (t) {
    case Type::TargetPeoplemon:
    case Type::Peopleball:
        return true;
    default:
        return false;
    }
}

bool Item::hasEffectOnPeoplemon(Id item, const pplmn::OwnedPeoplemon& ppl) {
    const auto checkStat = [&ppl](pplmn::Stat stat) -> bool {
        return ppl.currentEVs().sum() < pplmn::Stats::MaxEVSum &&
               ppl.currentEVs().get(stat) < pplmn::Stats::MaxEVStat;
    };

    switch (item) {
    case Id::Potion:
    case Id::SuperPotion:
    case Id::MegaPotion:
        return ppl.currentHp() < ppl.currentStats().hp && ppl.currentHp() > 0;
    case Id::SuperMegaUltraPotion:
        return (ppl.currentHp() < ppl.currentStats().hp ||
                ppl.currentAilment() != pplmn::Ailment::None) &&
               ppl.currentHp() > 0;

    case Id::PpPack:
    case Id::SuperPpPack:
        for (int i = 0; i < 4; ++i) {
            if (ppl.knownMoves()[i].id != pplmn::MoveId::Unknown &&
                ppl.knownMoves()[i].curPP < ppl.knownMoves()[i].maxPP) {
                return true;
            }
        }
        return false;
    case Id::Pp6Pack:
    case Id::SuperPp6Pack:
        return true; // just let them use it, dont check whole team
    case Id::PpRaiser:
        for (int i = 0; i < 4; ++i) {
            const auto& m = ppl.knownMoves()[i];
            if (m.id != pplmn::MoveId::Unknown) {
                if (m.maxPP < pplmn::Move::pp(m.id) * 8 / 5) { return true; }
            }
        }
        return false;

    case Id::KegOfProtein:
        return checkStat(pplmn::Stat::Attack);
    case Id::SuperPowerJuice:
        return checkStat(pplmn::Stat::SpecialAttack);
    case Id::TubOfIcedCream:
        return checkStat(pplmn::Stat::SpecialDefense);
    case Id::JarOfEncouragement:
        return checkStat(pplmn::Stat::Defense);
    case Id::SuperSpeedJuice:
        return checkStat(pplmn::Stat::Speed);
    case Id::Compliments:
        return checkStat(pplmn::Stat::HP);

    case Id::UnAnnoyerSoda:
        return ppl.currentAilment() == pplmn::Ailment::Annoyed;
    case Id::UnFrustratorSoda:
        return ppl.currentAilment() == pplmn::Ailment::Frustrated;
    case Id::WakeUpSoda:
        return ppl.currentAilment() == pplmn::Ailment::Sleep;
    case Id::UnStickySoda:
        return ppl.currentAilment() == pplmn::Ailment::Sticky;
    case Id::UnFreezeSoda:
        return ppl.currentAilment() == pplmn::Ailment::Frozen;

    default:
        return false;
    }
}

bool Item::hasEffectOnPeoplemon(Id item, const pplmn::BattlePeoplemon& ppl,
                                const battle::Battler& battler) {
    switch (item) {
    case Id::SuperMegaUltraPotion:
        if (battler.getSubstate().ailments != pplmn::PassiveAilment::None) return true;
        break;
    default:
        break;
    }
    return hasEffectOnPeoplemon(item, ppl.base());
}

void Item::useOnPeoplemon(Id item, pplmn::OwnedPeoplemon& ppl,
                          std::vector<pplmn::OwnedPeoplemon>* team,
                          std::vector<pplmn::BattlePeoplemon>* battleTeam) {
    if (!hasEffectOnPeoplemon(item, ppl)) return;

    const auto restorePP = [](pplmn::OwnedPeoplemon& ppl, int pp) {
        for (int i = 0; i < 4; ++i) {
            auto& m = ppl.knownMoves()[i];
            m.curPP = std::min(m.curPP + pp, m.maxPP);
        }
    };

    switch (item) {
    case Id::Potion:
        ppl.giveHealth(20);
        break;
    case Id::SuperPotion:
        ppl.giveHealth(50);
        break;
    case Id::MegaPotion:
        ppl.giveHealth(200);
        break;
    case Id::SuperMegaUltraPotion:
        ppl.giveHealth(9001);
        ppl.currentAilment() = pplmn::Ailment::None;
        break;

    case Id::PpPack:
        restorePP(ppl, 10);
        break;
    case Id::SuperPpPack:
        restorePP(ppl, 1000);
        break;
    case Id::Pp6Pack:
    case Id::SuperPp6Pack: {
        const int pp = item == Id::Pp6Pack ? 10 : 1000;
        if (battleTeam) {
            for (pplmn::BattlePeoplemon& p : *battleTeam) { restorePP(p.base(), pp); }
        }
        else if (team) {
            for (pplmn::OwnedPeoplemon& p : *team) { restorePP(p, pp); }
        }
        else {
            BL_LOG_ERROR << "Used PP 6 pack but not given team";
        }
        break;
    }
    case Id::PpRaiser:
        for (int i = 0; i < 4; ++i) {
            auto& m = ppl.knownMoves()[i];
            if (m.id != pplmn::MoveId::Unknown) {
                const int npp = std::min(m.maxPP + 1, pplmn::Move::pp(m.id) * 8 / 5);
                const int ppp = npp - m.maxPP;
                m.maxPP       = npp;
                m.curPP += ppp;
            }
        }
        break;

    case Id::UnAnnoyerSoda:
    case Id::UnFreezeSoda:
    case Id::UnFrustratorSoda:
    case Id::WakeUpSoda:
    case Id::UnStickySoda:
        ppl.currentAilment() = pplmn::Ailment::None;
        break;

    case Id::KegOfProtein:
        ppl.awardEVs(pplmn::Stats(pplmn::Stat::Attack, 10));
        break;
    case Id::SuperPowerJuice:
        ppl.awardEVs(pplmn::Stats(pplmn::Stat::SpecialAttack, 10));
        break;
    case Id::TubOfIcedCream:
        ppl.awardEVs(pplmn::Stats(pplmn::Stat::SpecialDefense, 10));
        break;
    case Id::JarOfEncouragement:
        ppl.awardEVs(pplmn::Stats(pplmn::Stat::Defense, 10));
        break;
    case Id::SuperSpeedJuice:
        ppl.awardEVs(pplmn::Stats(pplmn::Stat::Speed, 10));
        break;
    case Id::Compliments:
        ppl.awardEVs(pplmn::Stats(pplmn::Stat::HP, 10));
        break;

    default:
        break;
    }
}

void Item::useOnPeoplemon(Id item, pplmn::BattlePeoplemon& ppl, battle::Battler& battler) {
    if (!hasEffectOnPeoplemon(item, ppl, battler)) return;

    useOnPeoplemon(item, ppl.base(), nullptr, &battler.peoplemon());

    switch (item) {
    case Id::SuperMegaUltraPotion:
        battler.getSubstate().ailments = pplmn::PassiveAilment::None;
        break;
    default:
        break;
    }
}

std::string Item::getUseLine(Id item, const pplmn::OwnedPeoplemon& ppl) {
    switch (item) {
    case Id::Potion:
    case Id::SuperPotion:
    case Id::MegaPotion:
        return ppl.name() + " had it's HP restored.";
    case Id::SuperMegaUltraPotion:
        return ppl.name() + " was healed completely.";

    case Id::PpPack:
    case Id::SuperPpPack:
        return ppl.name() + " had it's PP restored!";
    case Id::Pp6Pack:
    case Id::SuperPp6Pack:
        return ppl.name() + " and all their friends had their PP restored.";
    case Id::PpRaiser:
        return ppl.name() + " had their PP raised.";

    case Id::KegOfProtein:
        return ppl.name() + " had their " + pplmn::Stats::statToString(pplmn::Stat::Attack) +
               " increased";
    case Id::SuperPowerJuice:
        return ppl.name() + " had their " + pplmn::Stats::statToString(pplmn::Stat::SpecialAttack) +
               " increased";
    case Id::TubOfIcedCream:
        return ppl.name() + " had their " +
               pplmn::Stats::statToString(pplmn::Stat::SpecialDefense) + " increased";
    case Id::JarOfEncouragement:
        return ppl.name() + " had their " + pplmn::Stats::statToString(pplmn::Stat::Defense) +
               " increased";
    case Id::SuperSpeedJuice:
        return ppl.name() + " had their " + pplmn::Stats::statToString(pplmn::Stat::Speed) +
               " increased";
    case Id::Compliments:
        return ppl.name() + " had their " + pplmn::Stats::statToString(pplmn::Stat::HP) +
               " increased";

    case Id::UnAnnoyerSoda:
        return ppl.name() + " is no longer Annoyed.";
    case Id::UnFrustratorSoda:
        return ppl.name() + " is no longer Frustrated.";
    case Id::WakeUpSoda:
        return ppl.name() + " is no longer Sleeping.";
    case Id::UnStickySoda:
        return ppl.name() + " is no longer Sticky.";
    case Id::UnFreezeSoda:
        return ppl.name() + " is no longer Frozen.";

    default:
        return "ERROR: " + getName(item) + " does not generate a useLine on a peoplemon.";
    }
}

bool Item::hasEffectOnPlayer(Id item, const player::State& state) {
    switch (item) {
    case Id::GoAwaySpray:
    case Id::SuperGoAwaySpray:
    case Id::NeverComeBackSpray:
        return state.repelSteps == 0;
    default:
        return false;
    }
}

void Item::useOnPlayer(Id item, player::State& state) {
    switch (item) {
    case Id::GoAwaySpray:
        state.repelSteps = 100;
        break;
    case Id::SuperGoAwaySpray:
        state.repelSteps = 250;
        break;
    case Id::NeverComeBackSpray:
        state.repelSteps = 2000;
        break;
    default:
        break;
    }
}

std::string Item::getUseLine(Id item) {
    switch (item) {
    case Id::GoAwaySpray:
    case Id::SuperGoAwaySpray:
    case Id::NeverComeBackSpray:
        return "Now you smell bad enough to keep Peoplemon away.";
    default:
        return "ERROR: " + getName(item) + " does not generate a useLine on the player.";
    }
}

pplmn::MoveId Item::getTmMove(Id tm) {
    using T    = std::underlying_type_t<Id>;
    const T id = static_cast<T>(tm);
    if (id < 200) return pplmn::MoveId::Unknown;
    const T mid = id - 200;
    return pplmn::Move::cast(mid);
}

} // namespace item
} // namespace core
