#ifndef GAME_BATTLES_MESSAGES_MESSAGE_HPP
#define GAME_BATTLES_MESSAGES_MESSAGE_HPP

#include <cstdint>
#include <string>

namespace game
{
namespace battle
{
class Message {
public:
    enum struct Type : std::uint8_t {
        Attack = 0,
        SuperEffective = 1,
        Callback = 2,
        SendOut = 3,
    };

private:
    //
};

} // namespace battle
} // namespace game

#endif
