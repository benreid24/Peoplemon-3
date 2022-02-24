#include <Game/Battles/Command.hpp>

namespace game
{
namespace battle
{
Command::Command(SyncState&& s)
: type(Type::SyncState)
, data(s) {}

Command::Command(const std::string& s)
: type(Type::DisplayMessage)
, data(s) {}

Command::Command(const Animation& a)
: type(Type::PlayAnimation)
, data(a) {}

} // namespace battle
} // namespace game
