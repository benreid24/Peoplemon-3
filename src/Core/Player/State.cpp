#include <Core/Player/State.hpp>

namespace core
{
namespace player
{
State::State()
: name("ERROR")
, sex(Gender::Boy)
, monei(0)
, peopledex(*this)
, whiteoutMap("Hometown/HometownYourHouseYourRoom.map")
, whiteoutSpawn(5)
, repelSteps(0) {}

void State::healPeoplemon() {
    for (auto& ppl : peoplemon) { ppl.heal(); }
}

bool State::evolutionPending() const {
    for (const auto& ppl : peoplemon) {
        if (ppl.pendingEvolution()) return true;
    }
    return false;
}

bool State::hasLivingPeoplemon() const {
    for (const auto& ppl : peoplemon) {
        if (ppl.currentHp() > 0) { return true; }
    }
    return false;
}

} // namespace player
} // namespace core
