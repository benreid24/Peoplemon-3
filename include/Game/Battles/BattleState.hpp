#ifndef GAME_BATTLES_BATTLESTATE_HPP
#define GAME_BATTLES_BATTLESTATE_HPP

#include <Game/Battles/Battler.hpp>
#include <Game/Battles/Command.hpp>
#include <queue>

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
     * @brief Creates a new BattleState
     *
     */
    BattleState();

    /**
     * @brief Updates the state and performs transitions based on battle logic and time elapsed
     *
     */
    virtual void update() = 0;

    /**
     * @brief Consumes a command emitted from the BattleFSM
     *
     * @param command The command to consume
     */
    virtual void consumeCommand(Command&& command);

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

protected:
    enum struct SubState : std::uint8_t {
        WaitingText = 1,
        WaitingAnim = 2,
        WaitingBars = 3,
        Done        = 4
    };

    // locally managed state
    std::queue<Command> commandQueue;
    SubState subState;

    // State synced with remote
    Battler* player;
    Battler* opponent;

    void processCommand(Command&& command);
    bool updateCommandQueue();
};

} // namespace battle
} // namespace game

#endif
