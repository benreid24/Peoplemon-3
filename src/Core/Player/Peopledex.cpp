#include <Core/Player/Peopledex.hpp>

#include <Core/Player/State.hpp>

namespace core
{
namespace player
{
Peopledex::Peopledex(State& state)
: playerState(state) {}

unsigned int Peopledex::getSeen(pplmn::Id ppl) const {
    const auto it = seenCounts.find(ppl);
    return it != seenCounts.end() ? it->second : 0;
}

const std::string& Peopledex::getFirstSeenLocation(pplmn::Id ppl) const {
    static const std::string empty;
    const auto it = firstSawLocations.find(ppl);
    return it != firstSawLocations.end() ? it->second : empty;
}

unsigned int Peopledex::getCaught(pplmn::Id ppl) const {
    unsigned int c = 0;

    for (const auto& p : playerState.peoplemon) {
        if (p.id() == ppl) ++c;
    }

    for (unsigned int i = 0; i < StorageSystem::BoxCount; ++i) {
        for (const auto& p : playerState.storage.getBox(i)) {
            if (p.peoplemon.id() == ppl) ++c;
        }
    }

    return c;
}

Peopledex::IntelLevel Peopledex::getIntelLevel(pplmn::Id ppl) const {
    const unsigned int s = getSeen(ppl);
    if (s == 0) return IntelLevel::NoIntel;

    const unsigned int c = getCaught(ppl);
    return c > 0 ? IntelLevel::FullIntel : IntelLevel::LimitedIntel;
}

void Peopledex::registerSighting(pplmn::Id ppl, const std::string& location) {
    auto it = seenCounts.find(ppl);
    if (it == seenCounts.end()) { it = seenCounts.try_emplace(ppl, 0).first; }
    it->second += 1;
    if (it->second == 1) { firstSawLocations[ppl] = location; }
}

} // namespace player
} // namespace core
