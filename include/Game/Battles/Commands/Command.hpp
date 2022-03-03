#ifndef GAME_BATTLES_MESSAGES_COMMAND_HPP
#define GAME_BATTLES_MESSAGES_COMMAND_HPP

#include <Game/Battles/Commands/Animation.hpp>
#include <Game/Battles/Commands/Message.hpp>
#include <cstdint>
#include <string>
#include <variant>

namespace game
{
namespace battle
{
/**
 * @brief Emitted by the BattleFSM. All UI is handled via a queue of commands. Commands are resolved
 *        in the order they are received. Commands are sent over the network and each player
 *        individually resolves them.
 *
 * @ingroup Battles
 *
 */
class Command {
public:
    /**
     * @brief The type the command is
     *
     */
    enum struct Type : std::uint8_t {
        DisplayMessage = 0,
        PlayAnimation  = 1,
        SyncState      = 2 // sync with view. also for network sync
        // TODO - others? maybe use commands for battler choices for easy networking?
    };

    /**
     * @brief Empty struct to create sync commands
     *
     */
    struct SyncState {};

    /**
     * @brief Creates a new sync command. This is emitted whenever the BattleState is modified and
     *        needs to be sent over the network
     *
     */
    Command(SyncState&& unused, bool waitView);

    /**
     * @brief Creates a new DisplayMessage command
     *
     * @param message The message to display
     */
    Command(Message&& message, bool waitView);

    /**
     * @brief Creates a new PlayAnimation command
     *
     * @param anim The animation descriptor to play
     */
    Command(Animation&& anim, bool waitView);

    /**
     * @brief Returns the type of this command
     *
     */
    Type getType() const;

    /**
     * @brief Returns the message if this command is a message
     *
     */
    const Message& getMessage() const;

    /**
     * @brief Returns the animation if this command is an animation
     *
     */
    const Animation& getAnimation() const;

    /**
     * @brief True if the battle controller should wait for the view or false to continue to process
     *        commands
     *
     */
    bool waitForView() const;

private:
    const Type type;
    const std::variant<Message, Animation, SyncState> data;
    const bool wait;
};

} // namespace battle
} // namespace game

#endif
