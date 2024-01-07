#include <Core/Files/Trainer.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{
Trainer::Trainer()
: payout(0) {}

bool Trainer::save(const std::string& file) const {
    std::ofstream output(file.c_str());
    return bl::serial::json::Serializer<Trainer>::serializeStream(output, *this, 4, 0);
}

bool Trainer::saveBundle(bl::serial::binary::OutputStream& output,
                         bl::resource::bundle::FileHandlerContext& ctx) const {
    if (!bl::serial::binary::Serializer<Trainer>::serialize(output, *this)) return false;

    const std::array<std::string, 4> Suffixes{"up.anim", "right.anim", "down.anim", "left.anim"};
    const std::string ap =
        bl::util::FileUtil::joinPath(Properties::CharacterAnimationPath(), animation);
    for (const std::string& s : Suffixes) {
        const std::string p = bl::util::FileUtil::joinPath(ap, s);
        if (bl::util::FileUtil::exists(p)) { ctx.addDependencyFile(p); }
        else { BL_LOG_WARN << "Trainer " << name << " is missing anim: " << p; }
    }

    const auto addConv = [this, &ctx](const std::string& cf) {
        const std::string c = bl::util::FileUtil::joinPath(Properties::ConversationPath(), cf);
        if (bl::util::FileUtil::exists(c)) { ctx.addDependencyFile(c); }
        else { BL_LOG_WARN << "Trainer " << name << " is missing conversation " << cf; }
    };
    addConv(prebattleConversation);
    addConv(postBattleConversation);

    return true;
}

bool Trainer::load(const std::string& file, bl::tmap::Direction spawnDir) {
    sourceFile = file;
    if (!TrainerManager::initializeExisting(file, *this)) return false;
    if (behavior.type() == Behavior::StandStill) { behavior.standing().facedir = spawnDir; }
    if (payout == 0) { payout = 40; }
    for (auto& ppl : peoplemon) { ppl.heal(); }
    return true;
}

bool Trainer::loadProd(bl::serial::binary::InputStream& input) {
    return bl::serial::binary::Serializer<Trainer>::deserialize(input, *this);
}

bool Trainer::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<Trainer>::deserializeStream(input, *this);
}

} // namespace file
} // namespace core
