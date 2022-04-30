#include <Core/Battles/Commands/Message.hpp>

namespace core
{
namespace battle
{
namespace cmd
{
Message::Message(Type tp)
: type(tp)
, data(Empty())
, forActive(true) {}

Message::Message(pplmn::MoveId mid)
: type(Type::Attack)
, data(mid)
, forActive(true) {}

Message::Message(Type tp, bool b)
: type(tp)
, data(Empty())
, forActive(b) {}

Message::Message(Type tp, pplmn::Ailment ail, bool a)
: type(tp)
, data(ail)
, forActive(a) {}

Message::Message(Type tp, pplmn::PassiveAilment ail, bool a)
: type(tp)
, data(ail)
, forActive(a) {}

Message::Message(Type tp, pplmn::Stat stat, bool a)
: type(tp)
, data(stat)
, forActive(a) {}

Message::Type Message::getType() const { return type; }

pplmn::MoveId Message::getMoveId() const {
    const pplmn::MoveId* mid = std::get_if<pplmn::MoveId>(&data);
    return mid ? *mid : pplmn::MoveId::Unknown;
}

bool Message::forActiveBattler() const { return forActive; }

pplmn::Ailment Message::getAilment() const {
    const pplmn::Ailment* a = std::get_if<pplmn::Ailment>(&data);
    return a ? *a : pplmn::Ailment::None;
}

pplmn::PassiveAilment Message::getPassiveAilment() const {
    const pplmn::PassiveAilment* a = std::get_if<pplmn::PassiveAilment>(&data);
    return a ? *a : pplmn::PassiveAilment::None;
}

pplmn::Stat Message::getStat() const {
    const pplmn::Stat* s = std::get_if<pplmn::Stat>(&data);
    return s ? *s : pplmn::Stat::Attack;
}

} // namespace cmd
} // namespace battle
} // namespace core
