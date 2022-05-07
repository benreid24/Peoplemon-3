#ifndef GAME_BATTLES_BATTLESTATE_HPP
#define GAME_BATTLES_BATTLESTATE_HPP

#include <Core/Battles/Battler.hpp>

namespace core
{
namespace battle
{
/**
 * @brief Stores and represents the current state of a battle. This is the base class for local and
 *        remote battle states and provides the data storage as well as the local command queue
 *        resolution.
 *
 * @ingroup Battles
 *
 */
class BattleState {
public:
    /**
     * @brief Represents all possible states in a battle
     *
     */
    enum struct Stage : std::uint8_t {
        // Battle intro
        WildIntro = 0,
        TrainerIntro,
        NetworkIntro,
        IntroSendInSelf,
        IntroSendInOpponent,

        // Turn and round flow
        WaitingChoices,
        RoundStart,
        TurnStart,
        NextBattler,
        RoundFinalEffects,
        RoundEnd,

        // Use item
        PreUseItem,
        UsingItem,
        PostUseItem,

        // TODO - wild peoplemon ball states

        // Switch out
        BeforeSwitch,
        Switching,
        AfterSwitch,

        // Run
        BeforeRun,
        Running,
        RunFailed,

        // Fight
        Attacking,

        // Various switch states
        WaitingMidTurnSwitch,
        BeforeMidTurnSwitch,
        MidTurnSwitching,
        AfterMidTurnSwitch,
        RoarSwitching,
        AfterRoarSwitch,

        // Peoplemon defeated
        Fainting,
        CheckPlayerContinue,
        WaitingPlayerContinue,
        CheckFaint,
        WaitingFaintSwitch,
        FaintSwitching,
        AfterFaintSwitch,

        // XP and level up
        XpAwardBegin,
        XpAwardPeoplemonBegin,
        XpAwarding,
        LevelingUp,
        WaitingLearnMoveChoice,
        WaitingForgetMoveChoice,

        // Battle end
        NetworkDefeated,
        NetworkLost,
        TrainerDefeated,
        PeoplemonCaught,
        Whiteout,

        // Final states
        Completed,
        NetworkDisconnect
    };

    /**
     * @brief Creates a new BattleState
     *
     * @param initialState The initial stage to start with. Must be an intro stage
     *
     */
    BattleState(Stage initialState);

    /**
     * @brief Begins the next round of turns and sets the battler order
     *
     * @param playerIsFirst True if the local player goes first
     */
    void beginRound(bool playerIsFirst);

    /**
     * @brief Moves on to the next battler's turn. Returns the stage to transition to
     *
     * @return Stage Either WaitingChoices or TurnStart
     */
    Stage nextTurn();

    /**
     * @brief Returns whether the current mover is the first peoplemon to go this round
     *
     */
    bool isFirstMover() const;

    /**
     * @brief Returns the local player Battler
     *
     */
    Battler& localPlayer();

    /**
     * @brief Returns the opponent Battler
     *
     */
    Battler& enemy();

    /**
     * @brief Returns the battler that is current resolving their turn
     *
     */
    Battler& activeBattler();

    /**
     * @brief Returns the battler who is currently not resolving their turn
     *
     */
    Battler& inactiveBattler();

    /**
     * @brief Returns the current stage the battle is in
     *
     */
    Stage currentStage() const;

    /**
     * @brief Sets the current stage of the battle
     *
     * @param stage The new battle stage
     */
    void setStage(Stage stage);

private:
    Stage stage;
    Battler player;
    Battler opponent; // TODO - some way to swap for network client?
    std::uint8_t currentMover;
    std::uint8_t firstMover;
};

} // namespace battle
} // namespace core

#endif
