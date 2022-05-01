#ifndef GAME_BATTLES_COMMANDS_COMMAND_HPP
#define GAME_BATTLES_COMMANDS_COMMAND_HPP

#include <Core/Battles/Commands/Animation.hpp>
#include <Core/Battles/Commands/Message.hpp>
#include <cstdint>
#include <string>
#include <variant>

namespace core
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
    enum Type : std::uint8_t {
        DisplayMessage = 0,
        PlayAnimation  = 1,

        // sync with view. also for network sync
        SyncStateNoSwitch       = 2,
        SyncStatePlayerSwitch   = 3, // TODO - consider making active/inactive
        SyncStateOpponentSwitch = 4,
        WaitForView             = 5,

        GetBattlerChoices = 6,
        GetFaintSwitch    = 7,
        GetBatonSwitch    = 8
    };

    /**
     * @brief Creates a new command of the given type with no data
     *
     * @param type The command type to create
     */
    Command(Type type);

    /**
     * @brief Creates a new DisplayMessage command
     *
     * @param message The message to display
     */
    Command(cmd::Message&& message);

    /**
     * @brief Creates a new PlayAnimation command
     *
     * @param anim The animation descriptor to play
     */
    Command(cmd::Animation&& anim);

    /**
     * @brief Returns the type of this command
     *
     */
    Type getType() const;

    /**
     * @brief Returns the message if this command is a message
     *
     */
    const cmd::Message& getMessage() const;

    /**
     * @brief Returns the animation if this command is an animation
     *
     */
    const cmd::Animation& getAnimation() const;

private:
    struct Empty {};

    const Type type;
    const std::variant<Empty, cmd::Message, cmd::Animation> data;
};

} // namespace battle
} // namespace core

#endif
