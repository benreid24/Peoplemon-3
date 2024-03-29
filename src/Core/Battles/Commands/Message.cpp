#include <Core/Battles/Commands/Message.hpp>

namespace core
{
namespace battle
{
namespace cmd
{
using MovePair = std::pair<pplmn::MoveId, pplmn::MoveId>;

Message::Message(Type tp)
: type(tp)
, data(Empty())
, forHost(true) {}

Message::Message(pplmn::MoveId mid, bool fh)
: type(Type::Attack)
, data(mid)
, forHost(fh) {}

Message::Message(Type tp, bool b)
: type(tp)
, data(Empty())
, forHost(b) {}

Message::Message(Type tp, pplmn::Ailment ail, bool a)
: type(tp)
, data(ail)
, forHost(a) {}

Message::Message(Type tp, pplmn::PassiveAilment ail, bool a)
: type(tp)
, data(ail)
, forHost(a) {}

Message::Message(Type tp, pplmn::Stat stat, bool a)
: type(tp)
, data(stat)
, forHost(a) {}

Message::Message(Type tp, pplmn::MoveId og, pplmn::MoveId nm)
: type(tp)
, data(std::make_pair(og, nm))
, forHost(true) {}

Message::Message(Type tp, std::int16_t ival, bool a)
: type(tp)
, data(ival)
, forHost(a) {}

Message::Message(Type tp, std::uint8_t i, unsigned int iv, bool fh)
: type(tp)
, data(static_cast<std::uint16_t>(iv))
, forHost(fh)
, pplIndex(i) {}

Message::Message(Type tp, std::uint8_t i, pplmn::MoveId m, bool fh)
: type(tp)
, data(m)
, forHost(fh)
, pplIndex(i) {}

Message::Message(Type tp, pplmn::MoveId m, bool fh)
: type(tp)
, data(m)
, forHost(fh) {}

Message::Message(Type tp, item::Id item, bool a)
: type(tp)
, data(item)
, forHost(a) {}

Message::Message(Type tp, std::uint8_t i, item::Id item, bool fh)
: type(tp)
, data(item)
, forHost(fh)
, pplIndex(i) {}

Message::Type Message::getType() const { return type; }

pplmn::MoveId Message::getMoveId() const {
    const pplmn::MoveId* mid = std::get_if<pplmn::MoveId>(&data);
    return mid ? *mid : pplmn::MoveId::Unknown;
}

bool Message::forHostBattler() const { return forHost; }

std::uint8_t Message::forIndex() const { return pplIndex; }

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

pplmn::MoveId Message::getOriginalMove() const {
    const MovePair* mp = std::get_if<MovePair>(&data);
    return mp ? mp->first : pplmn::MoveId::Unknown;
}

pplmn::MoveId Message::getNewMove() const {
    const MovePair* mp = std::get_if<MovePair>(&data);
    return mp ? mp->second : pplmn::MoveId::Unknown;
}

std::int16_t Message::getInt() const {
    const std::int16_t* i = std::get_if<std::int16_t>(&data);
    return i ? *i : 1;
}

std::uint16_t Message::getUnsigned() const {
    const std::uint16_t* i = std::get_if<std::uint16_t>(&data);
    return i ? *i : 0;
}

item::Id Message::getItem() const {
    const item::Id* i = std::get_if<item::Id>(&data);
    return i ? *i : item::Id::None;
}

} // namespace cmd
} // namespace battle
} // namespace core
