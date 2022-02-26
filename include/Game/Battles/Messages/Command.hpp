#ifndef GAME_BATTLES_MESSAGES_COMMAND_HPP
#define GAME_BATTLES_MESSAGES_COMMAND_HPP

#include <Game/Battles/Messages/Animation.hpp>
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
        SyncState      = 2 // for network
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
    Command(SyncState&& unused);

    /**
     * @brief Creates a new DisplayMessage command
     *
     * @param message The message to display
     */
    Command(const std::string& message);

    /**
     * @brief Creates a new PlayAnimation command
     *
     * @param anim The animation descriptor to play
     */
    Command(const Animation& anim);

private:
    const Type type;
    std::variant<std::string, Animation, SyncState> data;
};

} // namespace battle
} // namespace game

#endif
