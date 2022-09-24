#include <Core/Peoplemon/Move.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace pplmn
{
namespace
{
const std::string Invalid = "<INVALID>";
std::vector<MoveId> allIds;
} // namespace
std::unordered_map<MoveId, std::string>* Move::names          = nullptr;
std::unordered_map<MoveId, std::string>* Move::descriptions   = nullptr;
std::unordered_map<MoveId, Type>* Move::types                 = nullptr;
std::unordered_map<MoveId, int>* Move::damages                = nullptr;
std::unordered_map<MoveId, int>* Move::accuracies             = nullptr;
std::unordered_map<MoveId, int>* Move::priorities             = nullptr;
std::unordered_map<MoveId, unsigned int>* Move::pps           = nullptr;
std::unordered_map<MoveId, bool>* Move::contactors            = nullptr;
std::unordered_map<MoveId, bool>* Move::specials              = nullptr;
std::unordered_map<MoveId, MoveEffect>* Move::effects         = nullptr;
std::unordered_map<MoveId, int>* Move::effectChances          = nullptr;
std::unordered_map<MoveId, int>* Move::effectIntensities      = nullptr;
std::unordered_map<MoveId, bool>* Move::effectSelves          = nullptr;

MoveId Move::cast(unsigned int id) {
    const MoveId r = static_cast<MoveId>(id);
    if (r == MoveId::_INVALID_1 || r == MoveId::_INVALID_2 || r == MoveId::_INVALID_3 ||
        r >= MoveId::_NUM_MOVES) {
        if (names->find(r) == names->end()) return MoveId::Unknown;
    }
    return r;
}

void Move::setDataSource(file::MoveDB& db) {
    names             = &db.names;
    descriptions      = &db.descriptions;
    types             = &db.types;
    damages           = &db.damages;
    accuracies        = &db.accuracies;
    priorities        = &db.priorities;
    pps               = &db.pps;
    contactors        = &db.contactors;
    specials          = &db.specials;
    effects           = &db.effects;
    effectChances     = &db.effectChances;
    effectIntensities = &db.effectIntensities;
    effectSelves      = &db.effectSelves;

    allIds.clear();
    allIds.reserve(names->size());
    for (const auto& p : *names) { allIds.emplace_back(p.first); }
}

const std::vector<MoveId>& Move::validIds() { return allIds; }

const std::string& Move::name(MoveId id) {
    const auto it = names->find(id);
    return it != names->end() ? it->second : Invalid;
}

const std::string& Move::description(MoveId id) {
    const auto it = descriptions->find(id);
    return it != descriptions->end() ? it->second : Invalid;
}

Type Move::type(MoveId id) {
    const auto it = types->find(id);
    return it != types->end() ? it->second : Type::None;
}

int Move::damage(MoveId id) {
    const auto it = damages->find(id);
    return it != damages->end() ? it->second : 0;
}

int Move::accuracy(MoveId id) {
    const auto it = accuracies->find(id);
    return it != accuracies->end() ? it->second : 0;
}

int Move::priority(MoveId id) {
    const auto it = priorities->find(id);
    return it != priorities->end() ? it->second : 0;
}

unsigned int Move::pp(MoveId id) {
    const auto it = pps->find(id);
    return it != pps->end() ? it->second : 0;
}

bool Move::makesContact(MoveId id) {
    const auto it = contactors->find(id);
    return it != contactors->end() ? it->second : false;
}

bool Move::isSpecial(MoveId id) {
    const auto it = specials->find(id);
    return it != specials->end() ? it->second : false;
}

MoveEffect Move::effect(MoveId id) {
    const auto it = effects->find(id);
    return it != effects->end() ? it->second : MoveEffect::None;
}

int Move::effectChance(MoveId id) {
    const auto it = effectChances->find(id);
    return it != effectChances->end() ? it->second : 0;
}

int Move::effectIntensity(MoveId id) {
    const auto it = effectIntensities->find(id);
    return it != effectIntensities->end() ? it->second : 0;
}

bool Move::affectsUser(MoveId id) {
    const auto it = effectSelves->find(id);
    return it != effectSelves->end() ? it->second : false;
}

std::string Move::playerAnimationBackground(MoveId move) {
    const std::string f1 = bl::util::FileUtil::joinPath(Properties::AnimationPath(), "Moves");
    const std::string f2 = bl::util::FileUtil::joinPath(f1, name(move));
    return bl::util::FileUtil::joinPath(f2, "Front/Background.anim");
}

std::string Move::playerAnimationForeground(MoveId move) {
    const std::string f1 = bl::util::FileUtil::joinPath(Properties::AnimationPath(), "Moves");
    const std::string f2 = bl::util::FileUtil::joinPath(f1, name(move));
    return bl::util::FileUtil::joinPath(f2, "Front/Foreground.anim");
}

std::string Move::opponentAnimationBackground(MoveId move) {
    const std::string f1 = bl::util::FileUtil::joinPath(Properties::AnimationPath(), "Moves");
    const std::string f2 = bl::util::FileUtil::joinPath(f1, name(move));
    return bl::util::FileUtil::joinPath(f2, "Back/Background.anim");
}

std::string Move::opponentAnimationForeground(MoveId move) {
    const std::string f1 = bl::util::FileUtil::joinPath(Properties::AnimationPath(), "Moves");
    const std::string f2 = bl::util::FileUtil::joinPath(f1, name(move));
    return bl::util::FileUtil::joinPath(f2, "Back/Foreground.anim");
}

MoveId Move::getRandomMove(bool ir) {
    MoveId ret;
    do {
        ret = allIds[bl::util::Random::get<std::size_t>(0, allIds.size() - 1)];
    } while (!ir && (effect(ret) == MoveEffect::RandomMove || effect(ret) == MoveEffect::Charge));
    return ret;
}

} // namespace pplmn
} // namespace core
