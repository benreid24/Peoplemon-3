#include <Core/Peoplemon/Move.hpp>

namespace core
{
namespace pplmn
{
std::unordered_map<MoveId, std::string>* Move::names        = nullptr;
std::unordered_map<MoveId, std::string>* Move::descriptions = nullptr;
std::unordered_map<MoveId, Type>* Move::types               = nullptr;
std::unordered_map<MoveId, int>* Move::damages              = nullptr;
std::unordered_map<MoveId, int>* Move::accuracies           = nullptr;
std::unordered_map<MoveId, int>* Move::priorities           = nullptr;
std::unordered_map<MoveId, unsigned int>* Move::pps         = nullptr;
std::unordered_map<MoveId, bool>* Move::contactors          = nullptr;
std::unordered_map<MoveId, bool>* Move::specials            = nullptr;
std::unordered_map<MoveId, MoveEffect>* Move::effects       = nullptr;
std::unordered_map<MoveId, int>* Move::effectChances        = nullptr;
std::unordered_map<MoveId, int>* Move::effectIntensities    = nullptr;
std::unordered_map<MoveId, bool>* Move::effectSelves        = nullptr;

MoveId Move::cast(unsigned int id) {
    const MoveId r = static_cast<MoveId>(id);
    if (r == MoveId::_INVALID_1 || r == MoveId::_INVALID_2 || r == MoveId::_INVALID_3 ||
        r >= MoveId::_NUM_MOVES) {
        if (names->find(r) == names->end()) return MoveId::Unknown;
    }
    return r;
}

// TODO - other stuff

} // namespace pplmn
} // namespace core
