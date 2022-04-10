#include <Core/Battles/Commands/Command.hpp>

namespace core
{
namespace battle
{
Command::Command(SyncStateNoSwitch&& s, bool w)
: type(Type::SyncStateNoSwitch)
, data(std::forward<SyncStateNoSwitch>(s))
, wait(w) {}

Command::Command(SyncStatePlayerSwitch&& s, bool w)
: type(Type::SyncStateNoSwitch)
, data(std::forward<SyncStatePlayerSwitch>(s))
, wait(w) {}

Command::Command(SyncStateOpponentSwitch&& s, bool w)
: type(Type::SyncStateNoSwitch)
, data(std::forward<SyncStateOpponentSwitch>(s))
, wait(w) {}

Command::Command(Message&& s, bool w)
: type(Type::DisplayMessage)
, data(std::forward<Message>(s))
, wait(w) {}

Command::Command(Animation&& a, bool w)
: type(Type::PlayAnimation)
, data(std::forward<Animation>(a))
, wait(w) {}

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

bool Command::waitForView() const { return wait; }

} // namespace battle
} // namespace core
