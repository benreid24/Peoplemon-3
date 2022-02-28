#include <Game/Battles/Commands/Command.hpp>

namespace game
{
namespace battle
{
Command::Command(SyncState&& s)
: type(Type::SyncState)
, data(std::forward<SyncState>(s)) {}

Command::Command(Message&& s)
: type(Type::DisplayMessage)
, data(std::forward<Message>(s)) {}

Command::Command(Animation&& a)
: type(Type::PlayAnimation)
, data(std::forward<Animation>(a)) {}

Command::Type Command::getType() const { return type; }

const Message& Command::getMessage() const {
    static const Message empty(Message::Type::_ERROR);
    const Message* m = std::get_if<Message>(&data);
    return m ? *m : empty;
}

const Animation& Command::getAnimation() const {
    static const Animation empty(Animation::Target::User, Animation::Type::_ERROR);
    const Animation* a = std::get_if<Animation>(&data);
    return a ? *a : empty;
}

} // namespace battle
} // namespace game
