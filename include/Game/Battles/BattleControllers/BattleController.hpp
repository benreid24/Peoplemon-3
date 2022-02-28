#ifndef GAME_BATTLES_BATTLECONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLER_HPP

#include <Game/Battles/Commands/Command.hpp>
#include <cstdint>
#include <queue>

namespace game
{
namespace battle
{
class BattleView;
class BattleState;

/**
 * @brief Interface for battle controllers. Battle controllers manage the flow of state and
 *        interaction for battles.
 *
 * @ingroup Battles
 *
 */
class BattleController {
public:
    /**
     * @brief Initializes some internal state
     *
     */
    BattleController();

    /**
     * @brief Destroy the Battle Controller object
     *
     */
    virtual ~BattleController() = default;

    /**
     * @brief Sets internal references to the view and state of the battle
     *
     * @param view The view of the battle
     * @param state The state of the battle
     */
    void init(BattleView& view, BattleState& state);

    /**
     * @brief Updates the processing of the command queue
     *
     */
    void update();

    /**
     * @brief Enqueues a command to manipulate battle state or the view
     *
     * @param command The command to enqueue
     */
    void queueCommand(Command&& command);

protected:
    BattleView* view;
    BattleState* state;

    /**
     * @brief Called when a command is first enqueued but not processed. May be used to send
     *        commands over the network, for example
     *
     * @param cmd The command that was just enqueued
     */
    virtual void onCommandQueued(const Command& cmd);

    /**
     * @brief This is called after a command is processed. Derived classes may perform specific
     *        actions for commands in this method, such as sending state across the network
     *
     * @param cmd The command that was just processed
     */
    virtual void onCommandProcessed(const Command& cmd) = 0;

    /**
     * @brief Derived controllers may use this method for update logic every frame
     *
     */
    virtual void onUpdate() = 0;

private:
    enum struct SubState : std::uint8_t { WaitingView = 1, Done = 2 };

    std::queue<Command> commandQueue;
    SubState subState;

    bool updateCommandQueue();
};
} // namespace battle
} // namespace game

#endif
