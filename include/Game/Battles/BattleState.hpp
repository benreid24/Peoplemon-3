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
        Intro = 0,

        WaitingChoices,

        UsingItem,

        BeforeSwitch,
        Switching,
        AfterSwitch,

        BeforeRun,
        Running,

        BeforeAttack,
        Attacking,
        AfterAttack,

        BeforeFaint,
        Fainting,
        XpAwarding,
        LevelingUp,
        BeforeFaintSwitch,
        FaintSwitching,
        AfterFaintSwitch,
        Victory,

        NetworkDisconnect
    };

    /**
     * @brief Creates a new BattleState
     *
     */
    BattleState();

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
