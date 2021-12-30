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
}

std::vector<Id> allIds;
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

    allIds.clear();
    allIds.reserve(names->size());
    for (const auto& p : *names) { allIds.emplace_back(p.first); }
    std::sort(allIds.begin(), allIds.end());
}

Id Peoplemon::cast(unsigned int id) {
    static constexpr unsigned int maxid = static_cast<unsigned int>(Id::_NUM_PEOPLEMON);
    const Id r                          = static_cast<Id>(id);
    if (id >= maxid && names->find(r) == names->end()) return Id::Unknown;
    return r;
}

const std::vector<Id>& Peoplemon::validIds() { return allIds; }

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

} // namespace pplmn
} // namespace core
