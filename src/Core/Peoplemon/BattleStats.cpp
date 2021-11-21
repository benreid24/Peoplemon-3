#include <Core/Peoplemon/BattleStats.hpp>

namespace core
{
namespace pplmn
{
BattleStats::BattleStats(bool z)
: acc(z ? 0 : 100)
, evade(z ? 0 : 100)
, crit(z ? 0 : 6) {}

BattleStats BattleStats::computeStats(const BattleStats& stats, const BattleStats& stages) {
    BattleStats result(true);
    result.acc   = stats.acc * Stats::stageMultiplier(stages.acc);
    result.evade = stats.evade * Stats::stageMultiplier(stages.evade);
    result.crit  = stages.crit > 0 ? (1 << stages.crit) : 1;
    return result;
}

} // namespace pplmn
} // namespace core
