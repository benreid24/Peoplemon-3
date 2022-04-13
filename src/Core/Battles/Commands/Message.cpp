#include <Core/Battles/Commands/Message.hpp>

namespace core
{
namespace battle
{
namespace cmd
{
Message::Message(Type tp)
: type(tp)
, data(Empty()) {}

Message::Message(Type tp, const std::string& str)
: type(tp)
, data(str) {}

Message::Type Message::getType() const { return type; }

const std::string& Message::getString() const {
    static const std::string err = "<ERROR>";
    return data.index() == 1 ? *std::get_if<std::string>(&data) : err;
}

} // namespace cmd
} // namespace battle
} // namespace core
