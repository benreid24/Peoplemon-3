#ifndef GAME_BATTLES_BATTLESTATE_HPP
#define GAME_BATTLES_BATTLESTATE_HPP

#include <Game/Battles/Battler.hpp>

namespace game
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

        // Turn start
        WaitingChoices,

        // Use item
        UsingItem,

        // Switch out
        BeforeSwitch,
        Switching,
        AfterSwitch,

        // Run
        BeforeRun,
        Running,

        // Fight
        BeforeAttack,
        Attacking,
        AfterAttack,

        // Peoplemon defeated
        BeforeFaint,
        Fainting,
        XpAwarding,
        LevelingUp,
        BeforeFaintSwitch,
        FaintSwitching,
        AfterFaintSwitch,

        // Battle end
        Victory,
        Lost,
        TrainerDefeated, // after Victory
        Completed,

        // Errors
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
     * @brief Returns the current stage the battle is in
     *
     */
    Stage currentStage() const;

private:
    Stage stage;
    Battler player;
    Battler opponent; // TODO - some way to swap for network client?
    std::uint8_t currentMover;
};

} // namespace battle
} // namespace game

#endif
