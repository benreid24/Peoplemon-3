#include <Core/Components/Trainer.hpp>

namespace core
{
namespace component
{
Trainer::Trainer(const file::Trainer& data)
: sourceFile(data.sourceFile)
, _name(data.name)
, loseDialog(data.lostBattleLine)
, visionRange(data.visionRange)
, peoplemon(data.peoplemon)
, _items(data.items)
, payout(data.payout)
, beat(false) {
    if (!beforeBattle.load(data.prebattleConversation)) {
        BL_LOG_ERROR << "Failed to load trainer before battle conversation: "
                     << data.prebattleConversation;
#ifdef PEOPLEMON_DEBUG
        beforeBattle = file::Conversation::makeLoadError(data.prebattleConversation);
#endif
    }
    if (!afterBattle.load(data.postBattleConversation)) {
        BL_LOG_ERROR << "Failed to load trainer after battle conversation: "
                     << data.postBattleConversation;
#ifdef PEOPLEMON_DEBUG
        afterBattle = file::Conversation::makeLoadError(data.postBattleConversation);
#endif
    }
}

const std::string& Trainer::file() const { return sourceFile; }

const std::string& Trainer::name() const { return _name; }

const file::Conversation& Trainer::beforeBattleConversation() const { return beforeBattle; }

const file::Conversation& Trainer::afterBattleConversation() const { return afterBattle; }

const std::string& Trainer::loseBattleDialog() const { return loseDialog; }

std::uint8_t Trainer::range() const { return visionRange; }

const std::vector<pplmn::OwnedPeoplemon>& Trainer::team() const { return peoplemon; }

const std::vector<item::Id>& Trainer::items() const { return _items; }

bool Trainer::defeated() const { return beat; }

void Trainer::setDefeated() { beat = true; }

std::uint8_t Trainer::basePayout() const { return payout; }

} // namespace component
} // namespace core
