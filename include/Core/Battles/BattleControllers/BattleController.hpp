#ifndef GAME_BATTLES_BATTLECONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLER_HPP

#include <Core/Battles/Commands/Command.hpp>
#include <cstdint>
#include <queue>

namespace core
{
namespace battle
{
struct Battle;
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
     * @param battle The battle itself
     * @param view The view of the battle
     * @param state The state of the battle
     */
    void init(Battle& battle, BattleView& view, BattleState& state);

    /**
     * @brief Updates the processing of the command queue
     *
     */
    void update();

    /**
     * @brief Enqueues a command to manipulate battle state or the view
     *
     * @param command The command to enqueue
     * @param addWait True to add a waitView command after the given command
     */
    void queueCommand(Command&& command, bool addWait = false);

protected:
    Battle* battle;
    BattleView* view;
    BattleState* state;

    /**
     * @brief Called when a command is first enqueued but not processed. May be used to send
     *        commands over the network, for example
     *
     * @param cmd The command that was just enqueued
     */
    virtual void onCommandQueued(const Command& cmd) = 0;

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
     * @param viewSynced Whether or not the view is fully up to date and static
     * @param queueEmpty Whether or not the command queue has been fully drained
     *
     */
    virtual void onUpdate(bool viewSynced, bool queueEmpty) = 0;

private:
    enum struct SubState : std::uint8_t { WaitingView = 1, Done = 2 };

    std::queue<Command> commandQueue;
    SubState subState;

    bool updateCommandQueue();
};
} // namespace battle
} // namespace core

#endif
