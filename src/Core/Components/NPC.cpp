#include <Core/Components/NPC.hpp>

namespace core
{
namespace component
{
NPC::NPC(const std::string& n, const file::Conversation& c)
: _name(n)
, _conversation(c) {}

const std::string& NPC::name() const { return _name; }

const file::Conversation& NPC::conversation() const { return _conversation; }

} // namespace component
} // namespace core
