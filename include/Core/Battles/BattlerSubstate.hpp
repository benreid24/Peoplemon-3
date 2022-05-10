#ifndef CORE_BATTLES_BATTLERSUBSTATE_HPP
#define CORE_BATTLES_BATTLERSUBSTATE_HPP

#include <Core/Battles/TurnAction.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/PassiveAilment.hpp>
#include <cstdint>

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
    std::int8_t lastMoveIndex;
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
    bool enduringThisTurn;
    bool enduredLastTurn;
    std::uint8_t spikesOut;
    std::int8_t healNext;
    bool move64Hit;
    std::int8_t copyStatsFrom;
    pplmn::PassiveAilment ailments;
    std::int8_t turnsConfused;
    std::int16_t koReviveHp;
    pplmn::MoveId lastMoveHitWith;
    std::uint16_t lastDamageTaken;
    bool faintHandled;
    component::Trainer* trainer;
    std::uint8_t turnsSticky;
    pplmn::MoveId lastMoveSuperEffective;
    bool preserveLastSuper;

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
     * @param outNow The peoplemon that is currently out
     *
     */
    void notifyTurnEnd(TurnAction action, pplmn::BattlePeoplemon& outNow);

    /**
     * @brief Updates state that is unique to the currently out Peoplemon
     *
     */
    void notifySwitch();

    /**
     * @brief Gives the peoplemon a passive ailment
     *
     * @param ail The ailment to give
     */
    void giveAilment(pplmn::PassiveAilment ail);

    /**
     * @brief Clear the given passive ailment
     *
     */
    void clearAilment(pplmn::PassiveAilment ail);

    /**
     * @brief Returns true if the peoplemon has the specific ailment
     *
     * @param ail The ailment to check for
     * @return True if the peoplemon has it, false otherwise
     */
    bool hasAilment(pplmn::PassiveAilment ail) const;
};
} // namespace battle
} // namespace core

#endif
