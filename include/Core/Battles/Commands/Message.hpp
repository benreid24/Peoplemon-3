#ifndef GAME_BATTLES_COMMANDS_MESSAGE_HPP
#define GAME_BATTLES_COMMANDS_MESSAGE_HPP

#include <Core/Peoplemon/MoveId.hpp>
#include <cstdint>
#include <string>
#include <variant>

namespace core
{
namespace battle
{
namespace cmd
{
/**
 * @brief Represents a message to be displayed. The actual message text is generated by the battle
 * controller
 *
 * @ingroup Battles
 *
 */
class Message {
public:
    /**
     * @brief The type of message to display
     *
     */
    enum struct Type : std::uint8_t {
        _ERROR = 0,
        Attack = 1,
        SuperEffective,
        Callback,
        SendOut,
        TrainerIntro,
        WildIntro,
        NetworkIntro,
        OpponentFirstSendout,
        PlayerFirstSendout,
        AttackMissed
    };

    /**
     * @brief Construct a new Message
     *
     * @param type The type of message to display
     */
    Message(Type type);

    /**
     * @brief Construct a new Message from a string
     *
     * @param type The type of message
     * @param str Data to help construct the message
     */
    Message(Type type, const std::string& str);

    /**
     * @brief Construct a new Message of the Attack type
     *
     * @param move The move being used
     */
    Message(pplmn::MoveId move);

    /**
     * @brief Returns the type of message this is
     *
     */
    Type getType() const;

    /**
     * @brief Returns the string data of the message
     *
     */
    const std::string& getString() const;

    /**
     * @brief Returns the move id of the message
     *
     */
    pplmn::MoveId getMoveId() const;

private:
    struct Empty {};

    const Type type;
    const std::variant<Empty, std::string, pplmn::MoveId> data;
};

} // namespace cmd
} // namespace battle
} // namespace core

#endif
