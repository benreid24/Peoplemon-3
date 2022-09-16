#include <Core/Maps/CatchRegion.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace core
{
namespace map
{
namespace
{
constexpr std::uint16_t FreqRange = std::numeric_limits<std::uint16_t>::max() - 1;
static const pplmn::WildPeoplemon Ben{pplmn::Id::BenToo, 100, 100, 100};
} // namespace

const pplmn::WildPeoplemon& CatchRegion::selectWild() const {
    if (wilds.empty()) {
        BL_LOG_ERROR << "Empty catch region, spawning your punishment";
        return Ben;
    }

    const std::uint16_t f = bl::util::Random::get<std::uint16_t>(0, FreqRange);
    std::uint16_t count   = 0;
    for (const pplmn::WildPeoplemon& ppl : wilds) {
        if (f < count + ppl.frequency) {
            BL_LOG_INFO << "Selected wild " << pplmn::Peoplemon::name(ppl.id) << "(" << ppl.id
                        << ")";
            return ppl;
        }
        count += ppl.frequency;
    }

    BL_LOG_ERROR
        << "Sum of wild peoplemon frequencies was less than 100% | Spawning your punishment";
    return Ben;
}

} // namespace map
} // namespace core
