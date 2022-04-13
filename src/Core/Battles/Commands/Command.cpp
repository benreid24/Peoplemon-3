#include <Core/Battles/Commands/Command.hpp>

namespace core
{
namespace battle
{
Command::Command(Type t)
: type(t)
, data(Empty()) {}

Command::Command(cmd::Message&& s)
: type(Type::DisplayMessage)
, data(std::forward<cmd::Message>(s)) {}

Command::Command(cmd::Animation&& a)
: type(Type::PlayAnimation)
, data(std::forward<cmd::Animation>(a)) {}

Command::Type Command::getType() const { return type; }

const cmd::Message& Command::getMessage() const {
    static const cmd::Message empty(cmd::Message::Type::_ERROR);
    const cmd::Message* m = std::get_if<cmd::Message>(&data);
    return m ? *m : empty;
}

const cmd::Animation& Command::getAnimation() const {
    static const cmd::Animation empty(cmd::Animation::Target::User, cmd::Animation::Type::_ERROR);
    const cmd::Animation* a = std::get_if<cmd::Animation>(&data);
    return a ? *a : empty;
}

} // namespace battle
} // namespace core
