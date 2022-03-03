#include <Game/Battles/Commands/Message.hpp>

namespace game
{
namespace battle
{
Message::Message(Type tp)
: type(tp) {}

Message::Type Message::getType() const { return type; }

} // namespace battle
} // namespace game
