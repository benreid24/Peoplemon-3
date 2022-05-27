#include <Core/Battles/Commands/Animation.hpp>

namespace core
{
namespace battle
{
namespace cmd
{
Animation::Animation(Type tp)
: forActive(true)
, type(tp)
, data(Empty()) {}

Animation::Animation(bool fa, Type tp)
: forActive(fa)
, type(tp)
, data(Empty()) {}

Animation::Animation(bool fa, core::pplmn::MoveId m)
: forActive(fa)
, type(Type::UseMove)
, data(m) {}

Animation::Animation(bool fa, Type tp, pplmn::Stat stat)
: forActive(fa)
, type(tp)
, data(stat) {}

Animation::Animation(bool fa, pplmn::Ailment ail)
: forActive(fa)
, type(Type::Ailment)
, data(ail) {}

Animation::Animation(bool fa, pplmn::PassiveAilment ail)
: forActive(fa)
, type(Type::PassiveAilment)
, data(ail) {}

bool Animation::forActiveBattler() const { return forActive; }

Animation::Type Animation::getType() const { return type; }

pplmn::MoveId Animation::getMove() const {
    const pplmn::MoveId* m = std::get_if<pplmn::MoveId>(&data);
    return m ? *m : pplmn::MoveId::Unknown;
}

pplmn::Stat Animation::getStat() const {
    const pplmn::Stat* s = std::get_if<pplmn::Stat>(&data);
    return s ? *s : pplmn::Stat::Attack;
}

pplmn::Ailment Animation::getAilment() const {
    const pplmn::Ailment* a = std::get_if<pplmn::Ailment>(&data);
    return a ? *a : pplmn::Ailment::Annoyed;
}

pplmn::PassiveAilment Animation::getPassiveAilment() const {
    const pplmn::PassiveAilment* a = std::get_if<pplmn::PassiveAilment>(&data);
    return a ? *a : pplmn::PassiveAilment::Confused;
}

} // namespace cmd
} // namespace battle
} // namespace core
