#include <Core/Peoplemon/WildPeoplemon.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace core
{
namespace pplmn
{
OwnedPeoplemon WildPeoplemon::generate() const {
    OwnedPeoplemon wild{id, bl::util::Random::get<unsigned int>(minLevel, maxLevel)};
    int mi = 0;
    for (unsigned int lvl = 1; lvl <= wild.currentLevel(); ++lvl) {
        const MoveId move = Peoplemon::moveLearnedAtLevel(wild.id(), lvl);
        if (move != MoveId::Unknown) {
            if (mi < 4) {
                wild.learnMove(move, mi);
                ++mi;
            }
            else {
                if (bl::util::Random::get<int>(0, 100) <= 40) {
                    wild.learnMove(move, bl::util::Random::get<unsigned int>(0, 3));
                }
            }
        }
    }
    return wild;
}

} // namespace pplmn
} // namespace core
