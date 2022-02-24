#ifndef GAME_BATTLES_BATTLEFSM_HPP
#define GAME_BATTLES_BATTLEFSM_HPP

#include <cstdint>

namespace game
{
namespace battle
{
class BattleState;
class Battler;

/**
 * @brief This is the meat and potatoes of the battle logic. This class is used to resolve moves,
 *        abilities, hold items, and everything else related to battle.
 *
 * @ingroup Battles
 *
 */
class BattleFSM {
public:
    /**
     * @brief Represents all possible states in the FSM (when coupled with the current battler)
     *
     */
    enum struct State : std::uint8_t {
        WaitingChoices = 0,

        UsingItem = 1,

        BeforeSwitch = 2,
        Switching    = 3,
        AfterSwitch  = 4,

        BeforeRun = 5,
        Running   = 6,

        BeforeAttack = 7,
        Attacking    = 8,
        AfterAttack  = 9,

        BeforeFaint      = 10,
        Fainting         = 11,
        XpAwarding       = 12,
        LevelingUp       = 13,
        FaintSwitching   = 14,
        AfterFaintSwitch = 15,
        Victory          = 16
    };

    /**
     * @brief Sets the initial state to WaitingChoices
     *
     */
    BattleFSM();

    /**
     * @brief Sets the battlers once they are available. Must be called before any other calls
     *
     * @param player The player battler
     * @param opponent The opponent battler
     */
    void init(Battler* player, Battler* opponent);

    /**
     * @brief Updates the state machine to the next state. This should be called when the command
     *        queue has been emptied after the previous call to update()
     *
     * @param state The battle state to operate on
     * @return True if update() should be called again, false if commands were emitted
     */
    bool update(BattleState& state);

    /**
     * @brief Returns the current state the FSM is in
     *
     */
    State currentState() const;

private:
    State state;
    Battler* order[2];
    std::uint8_t current;
};

} // namespace battle
} // namespace game

#endif
