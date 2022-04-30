#ifndef CORE_BATTLES_BATTLERSUBSTATE_HPP
#define CORE_BATTLES_BATTLERSUBSTATE_HPP

#include <Core/Battles/TurnAction.hpp>
#include <Core/Peoplemon/MoveId.hpp>

namespace core
{
namespace battle
{
/**
 * @brief Struct containing state specific to each battler. These states facilitate logic that takes
 *        place over multiple turns, such as the volleyball moves
 *
 * @ingroup Battle
 *
 */
struct BattlerSubstate {
    pplmn::MoveId lastMoveUsed;
    bool isProtected;

    /**
     * @brief Construct a new Battler State object with proper defaults
     *
     */
    BattlerSubstate();

    /**
     * @brief Resets and updates part of the state that depend on turns elapsing
     *
     * @param action The action the battler did on the last turn
     */
    void notifyTurn(TurnAction action);
};
} // namespace battle
} // namespace core

#endif
