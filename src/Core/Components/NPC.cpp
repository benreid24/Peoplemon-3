#include <Core/Components/NPC.hpp>

#include <Core/Properties.hpp>

namespace core
{
namespace component
{
NPC::NPC(const file::NPC& data)
: _name(data.name())
, _conversation() {
    if (!_conversation.load(
            bl::util::FileUtil::joinPath(Properties::ConversationPath(), data.conversation()))) {
        BL_LOG_ERROR << "Failed to load npc conversation: " << data.conversation();
#ifdef PEOPLEMON_DEBUG
        _conversation = file::Conversation::makeLoadError(data.conversation());
#endif
    }
}

const std::string& NPC::name() const { return _name; }

const file::Conversation& NPC::conversation() const { return _conversation; }

} // namespace component
} // namespace core
