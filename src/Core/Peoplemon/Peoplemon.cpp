#include <Core/Peoplemon/Peoplemon.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace pplmn
{
namespace
{
const std::string InvalidStr = "<INVALID>";
std::vector<Id> allIds;

void refreshIds(std::unordered_map<Id, std::string>* names) {
    allIds.clear();
    allIds.reserve(names->size());
    for (const auto& p : *names) { allIds.emplace_back(p.first); }
    std::sort(allIds.begin(), allIds.end());
}

const std::pair<std::string, std::string> abilityTexts[43] = {
    std::make_pair("No Ability", "This Peoplemon has no special talents to speak of"),
    std::make_pair("Board Game Master", "The opposing Peoplemon cannot switch out"),
    std::make_pair("Chillaxed", "Cannot receive critical hits"),
    std::make_pair("Classy", "Cannot be Frustrated"),
    std::make_pair("Goon", "Enemy Peoplemon take damage when they attack physically"),
    std::make_pair("Quick Draw", "Attacks first on the first turn of every battle"),
    std::make_pair("Always Friendly", "Is cured of all ailments when switched out"),
    std::make_pair("Skipped Slot", "Id #7 is missing, it's Chris' fault"),
    std::make_pair("Impulse Buy", "May purchase items at random in stores"),
    std::make_pair("Run Away", "Always able to get away from battle"),
    std::make_pair("Mr. Extra", "Occasionally finds items when exploring and pockets them"),
    std::make_pair("Beefed Up", "Athletic type moves do more damage when below 25% HP"),
    std::make_pair("Reserved", "Quiet type moves do more damage when below 25% HP"),
    std::make_pair("Duke of Jokes", "Joke based moves do 50% more damage"),
    std::make_pair(
        "Engaging",
        "Moves that otherwise would not affect enemy Peoplemon become Not Very Effective instead"),
    std::make_pair("Snack Share", "Chance to give inflicted ailments to the attacker as well"),
    std::make_pair("Derp Derp", "Chance to Confuse opponent when taking damage"),
    std::make_pair("Klutz",
                   "Chance to drop hold item before battle, making it unusable for the battle"),
    std::make_pair("Gender Bend", "Changes the gender of enemy Peoplemon"),
    std::make_pair("Garbage", "This Peoplemon is garbage and has no special skills"),
    std::make_pair("Bike Mechanic", "Can fix broken bikes"),
    std::make_pair("Sidetrack Teach", "Teach based moves have a chance to Distract opponents"),
    std::make_pair("No Joke Teach",
                   "Takes no damage from Joke based moves if a Teach based move was used"),
    std::make_pair("Fiery Teach", "Teach based moves have a chance to raise Special Attack"),
    std::make_pair("Experienced Teach", "Cannot be K.O'd if a Teach based move was used"),
    std::make_pair("New Teach",
                   "Teach based moves used on the first turn of a battle have higher speed"),
    std::make_pair("Doze Off",
                   "Teach based moves have a chance to make this Peoplemon fall asleep"),
    std::make_pair("Douse Flames",
                   "Has a chance to reduce opponent Attack after taking physical damage"),
    std::make_pair("Flirty",
                   "Has a chance to reduce opponent Special Attack after taking special damage"),
    std::make_pair("Undying Faith", "Has a chance to survive a lethal attack"),
    std::make_pair("Too Cool", "Cannot be Stunned"),
    std::make_pair("Fake Study",
                   "If an attack misses the move used is not revealed to the opponent"),
    std::make_pair("Alcoholic", "Consumes Alcohol based items at the beginning of each turn"),
    std::make_pair("Total Bro", "Increases Bro Power of other Bros in your party"),
    std::make_pair("Total Mom", "Restores some PP of the next Peoplemon when K.O'd"),
    std::make_pair("Can't Swim",
                   "Has a chance to decrease opponents Attack when hit with a physical move"),
    std::make_pair("All Nighter", "Cannot be put to sleep"),
    std::make_pair("Ailment Saturated", "Can only ever have one Ailment at a time"),
    std::make_pair("Adamant", "Can't be forced to switch out of battle by an opponent"),
    std::make_pair("Absolute Pitch", "Super Effective moves do additional damage, but Not Very "
                                     "Effective moves do even less damage"),
    std::make_pair("GameMaker Virus", "Eliminates half of the PP of any moves that it gets hit by"),
    std::make_pair("Snapshot",
                   "Cannot take Super Effective damage from the same move twice in a row"),
    std::make_pair("Get Baked", "Automatically self-destructs if at 20% or less HP")};
} // namespace

std::unordered_map<Id, std::string>* Peoplemon::names                                     = nullptr;
std::unordered_map<Id, std::string>* Peoplemon::descriptions                              = nullptr;
std::unordered_map<Id, Type>* Peoplemon::types                                            = nullptr;
std::unordered_map<Id, SpecialAbility>* Peoplemon::abilities                              = nullptr;
std::unordered_map<Id, Stats>* Peoplemon::stats                                           = nullptr;
std::unordered_map<Id, std::unordered_set<MoveId>>* Peoplemon::validMoves                 = nullptr;
std::unordered_map<Id, std::unordered_map<unsigned int, MoveId>>* Peoplemon::learnedMoves = nullptr;
std::unordered_map<Id, unsigned int>* Peoplemon::evolveLevels                             = nullptr;
std::unordered_map<Id, Id>* Peoplemon::evolveIds                                          = nullptr;
std::unordered_map<Id, Stats>* Peoplemon::evAwards                                        = nullptr;
std::unordered_map<Id, unsigned int>* Peoplemon::xpGroups                                 = nullptr;
std::unordered_map<Id, int>* Peoplemon::xpMults                                           = nullptr;
std::unordered_map<Id, int>* Peoplemon::catchRates                                        = nullptr;

void Peoplemon::setDataSource(file::PeoplemonDB& db) {
    names        = &db.names;
    descriptions = &db.descriptions;
    types        = &db.types;
    abilities    = &db.abilities;
    stats        = &db.stats;
    validMoves   = &db.validMoves;
    learnedMoves = &db.learnedMoves;
    evolveLevels = &db.evolveLevels;
    evolveIds    = &db.evolveIds;
    evAwards     = &db.evAwards;
    xpGroups     = &db.xpGroups;
    xpMults      = &db.xpMults;
    catchRates   = &db.catchRates;

    refreshIds(names);
}

Id Peoplemon::cast(unsigned int id) {
    const Id r = static_cast<Id>(id);
    if (names != nullptr && names->find(r) == names->end()) return Id::Unknown;
    return r;
}

const std::vector<Id>& Peoplemon::validIds() {
#ifdef PEOPLEMON_DEBUG
    refreshIds(names);
#endif

    return allIds;
}

const std::string& Peoplemon::name(Id id) {
    const auto it = names->find(id);
    return it != names->end() ? it->second : InvalidStr;
}

const std::string& Peoplemon::description(Id id) {
    const auto it = descriptions->find(id);
    return it != descriptions->end() ? it->second : InvalidStr;
}

Type Peoplemon::type(Id id) {
    const auto it = types->find(id);
    return it != types->end() ? it->second : Type::None;
}

SpecialAbility Peoplemon::specialAbility(Id id) {
    const auto it = abilities->find(id);
    return it != abilities->end() ? it->second : SpecialAbility::None;
}

const Stats& Peoplemon::baseStats(Id id) {
    static const Stats Zero;
    const auto it = stats->find(id);
    return it != stats->end() ? it->second : Zero;
}

bool Peoplemon::canLearnMove(Id id, MoveId move) {
    const auto it = validMoves->find(id);
    return it != validMoves->end() ? it->second.find(move) != it->second.end() : false;
}

MoveId Peoplemon::moveLearnedAtLevel(Id id, unsigned int level) {
    const auto it = learnedMoves->find(id);
    if (it != learnedMoves->end()) {
        const auto jit = it->second.find(level);
        return jit != it->second.end() ? jit->second : MoveId::Unknown;
    }
    return MoveId::Unknown;
}

unsigned int Peoplemon::evolveLevel(Id id) {
    const auto it = evolveLevels->find(id);
    return it != evolveLevels->end() ? it->second : 101;
}

Id Peoplemon::evolvesInto(Id id) {
    const auto it = evolveIds->find(id);
    return it != evolveIds->end() ? it->second : Id::Unknown;
}

const Stats& Peoplemon::evAward(Id id) {
    static const Stats Zero;
    const auto it = evAwards->find(id);
    return it != evAwards->end() ? it->second : Zero;
}

int Peoplemon::xpYieldMultiplier(Id id) {
    const auto it = xpMults->find(id);
    return it != xpMults->end() ? it->second : 1;
}

unsigned int Peoplemon::levelUpXp(Id id, unsigned int level) {
    const auto it = xpGroups->find(id);
    if (it == xpGroups->end()) return std::numeric_limits<unsigned int>::max();
    switch (it->second) {
    case 0:
        return 5 * level * level * level / 4;
    case 1:
        return level * level * level;
    case 2:
        return 4 * level * level * level / 5;
    default:
        BL_LOG_ERROR << "Invalid xp group " << it->second << " for peoplemon "
                     << static_cast<unsigned int>(id);
        return std::numeric_limits<unsigned int>::max();
    }
}

Id Peoplemon::evolvesFrom(Id id) {
    for (const auto& p : *evolveIds) {
        if (p.second == id) return p.first;
    }
    return Id::Unknown;
}

std::string Peoplemon::thumbnailImage(Id id) {
    static const std::string missingno =
        bl::util::FileUtil::joinPath(Properties::PeoplemonImageFolder(), "0.png");
    const std::string f = bl::util::FileUtil::joinPath(
        Properties::PeoplemonImageFolder(), std::to_string(static_cast<unsigned int>(id)) + ".png");
    return bl::util::FileUtil::exists(f) ? f : missingno;
}

std::string Peoplemon::playerImage(Id id) {
    static const std::string rd =
        bl::util::FileUtil::joinPath(Properties::ImagePath(), "Battle/Peoplemon/Player");
    static const std::string missingno = bl::util::FileUtil::joinPath(rd, "0.png");
    const std::string img =
        bl::util::FileUtil::joinPath(rd, std::to_string(static_cast<unsigned int>(id)) + ".png");
    return bl::util::FileUtil::exists(img) ? img : missingno;
}

std::string Peoplemon::opponentImage(Id id) {
    static const std::string rd =
        bl::util::FileUtil::joinPath(Properties::ImagePath(), "Battle/Peoplemon/Opponent");
    static const std::string missingno = bl::util::FileUtil::joinPath(rd, "0.png");
    const std::string img =
        bl::util::FileUtil::joinPath(rd, std::to_string(static_cast<unsigned int>(id)) + ".png");
    return bl::util::FileUtil::exists(img) ? img : missingno;
}

float Peoplemon::catchRate(Id id) {
    const auto it = catchRates->find(id);
    return it != catchRates->end() ? static_cast<float>(it->second) : 48.f;
}

bool Peoplemon::canClone(Id id) { return id != Id::Ben && id != Id::BenToo; }

std::string Peoplemon::ailmentString(Ailment ail) {
    switch (ail) {
    case Ailment::Annoyed:
        return "Annoyed";
    case Ailment::Frozen:
        return "Frozen";
    case Ailment::Frustrated:
        return "Frustrated";
    case Ailment::None:
        return "Healthy";
    case Ailment::Sleep:
        return "Sleep";
    case Ailment::Sticky:
        return "Sticky";
    default:
        return "???";
    }
}

const std::string& Peoplemon::abilityName(SpecialAbility ability) {
    const unsigned int i = static_cast<unsigned int>(ability);
    return i < std::size(abilityTexts) ? abilityTexts[i].first : InvalidStr;
}

const std::string& Peoplemon::abilityDescription(SpecialAbility ability) {
    const unsigned int i = static_cast<unsigned int>(ability);
    return i < std::size(abilityTexts) ? abilityTexts[i].second : InvalidStr;
}

} // namespace pplmn
} // namespace core
