#include <Core/Peoplemon/BattleStats.hpp>

namespace core
{
namespace pplmn
{
BattleStats::BattleStats(bool z)
: acc(z ? 0 : 100)
, evade(z ? 0 : 100)
, crit(z ? 0 : 6) {}

float BattleStats::getAccuracyMultiplier(int stage) {
    switch (stage) {
    case -6:
        return 0.33f;
    case -5:
        return 0.36f;
    case -4:
        return 0.43f;
    case -3:
        return 0.50f;
    case -2:
        return 0.60f;
    case -1:
        return 0.75f;
    case 1:
        return 1.33f;
    case 2:
        return 1.66f;
    case 3:
        return 2.f;
    case 4:
        return 2.33f;
    case 5:
        return 2.66f;
    case 6:
        return 3.f;
    case 0:
    default:
        return 1.f;
    }
}

} // namespace pplmn
} // namespace core
