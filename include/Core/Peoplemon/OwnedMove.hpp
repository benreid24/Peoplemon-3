#ifndef CORE_PEOPLEMON_OWNEDMOVE_HPP
#define CORE_PEOPLEMON_OWNEDMOVE_HPP

#include <Core/Peoplemon/MoveId.hpp>

namespace core
{
namespace pplmn
{
/**
 * @brief Represents a move owned by a peoplemon
 *
 * @ingroup Peoplemon
 *
 */
struct OwnedMove {
    /// The id of the move
    MoveId id;

    /// The current pp of the move
    unsigned int curPP;

    /// The pp to restore to. Increased max pp is stored here
    unsigned int maxPP;

    /**
     * @brief Creates an empty Unknown move with 0 pp
     *
     */
    OwnedMove();

    /**
     * @brief Creates the move from the given id and sets PP accordingly
     *
     * @param id The move to set to
     */
    OwnedMove(MoveId id);
};

} // namespace pplmn
} // namespace core

#endif
