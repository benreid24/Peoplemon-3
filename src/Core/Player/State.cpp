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

} // namespace player
} // namespace core
