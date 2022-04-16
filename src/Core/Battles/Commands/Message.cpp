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

Message::Message(pplmn::MoveId mid)
: type(Type::Attack)
, data(mid) {}

Message::Type Message::getType() const { return type; }

const std::string& Message::getString() const {
    static const std::string err = "<ERROR>";
    return data.index() == 1 ? *std::get_if<std::string>(&data) : err;
}

pplmn::MoveId Message::getMoveId() const {
    const pplmn::MoveId* mid = std::get_if<pplmn::MoveId>(&data);
    return mid ? *mid : pplmn::MoveId::Unknown;
}

} // namespace cmd
} // namespace battle
} // namespace core
