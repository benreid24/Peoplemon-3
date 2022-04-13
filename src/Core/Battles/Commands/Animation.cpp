#include <Core/Battles/Commands/Animation.hpp>

namespace core
{
namespace battle
{
namespace cmd
{
Animation::Animation(Target targ, Type tp)
: target(targ)
, type(tp)
, move(core::pplmn::MoveId::Unknown) {}

Animation::Animation(Target targ, core::pplmn::MoveId m)
: target(targ)
, type(Type::UseMove)
, move(m) {}

} // namespace cmd
} // namespace battle
} // namespace core
