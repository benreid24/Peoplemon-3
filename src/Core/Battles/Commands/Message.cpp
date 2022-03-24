#include <Core/Battles/Commands/Message.hpp>

namespace core
{
namespace battle
{
Message::Message(Type tp)
: type(tp) {}

Message::Type Message::getType() const { return type; }

} // namespace battle
} // namespace core
