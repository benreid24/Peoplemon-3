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
    std::int16_t substituteHp;
    std::uint8_t turnsGuarded;
    std::int8_t chargingMove;
    std::int8_t encoreMove;
    std::uint8_t encoreTurnsLeft;
    std::int8_t deathCounter;

    bool ballBlocked;
    bool ballBumped;
    bool ballSet;
    bool ballSpiked;
    bool ballSwiped;
    bool noOneToGetBall;
    std::int8_t ballUpTurns;

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
    void notifyTurnBegin();

    /**
     * @brief Resets and updates part of the state that depend on turns elapsing
     *
     * @param action The action the battler did on the last turn
     *
     */
    void notifyTurnEnd(TurnAction action);

    /**
     * @brief Updates state that is unique to the currently out Peoplemon
     *
     * @param fromFaint True if the switch is from fainting, false for regular
     *
     */
    void notifySwitch(bool fromFaint);
};
} // namespace battle
} // namespace core

#endif
