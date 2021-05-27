#include <Core/Components/Trainer.hpp>

namespace core
{
namespace component
{
Trainer::Trainer(const file::Trainer& data)
: _name(data.name())
, loseDialog(data.lostBattleLine()) {
    if (!beforeBattle.load(data.prebattleConversation())) {
        BL_LOG_ERROR << "Failed to load trainer before battle conversation: "
                     << data.prebattleConversation();
#ifdef PEOPLEMON_DEBUG
        beforeBattle = file::Conversation::makeLoadError(data.prebattleConversation());
#endif
    }
    if (!afterBattle.load(data.postBattleConversation())) {
        BL_LOG_ERROR << "Failed to load trainer after battle conversation: "
                     << data.postBattleConversation();
#ifdef PEOPLEMON_DEBUG
        afterBattle = file::Conversation::makeLoadError(data.postBattleConversation());
#endif
    }
}

const std::string& Trainer::name() const { return _name; }

const file::Conversation& Trainer::beforeBattleConversation() const { return beforeBattle; }

const file::Conversation& Trainer::afterBattleConversation() const { return afterBattle; }

const std::string& Trainer::loseBattleDialog() const { return loseDialog; }

} // namespace component
} // namespace core
