#include <Core/Files/NPC.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{
bool NPC::save(const std::string& file) const {
    std::ofstream output(file.c_str());
    return bl::serial::json::Serializer<NPC>::serializeStream(output, *this, 4, 0);
}

bool NPC::saveBundle(bl::serial::binary::OutputStream& output,
                     bl::resource::bundle::FileHandlerContext& ctx) const {
    if (!bl::serial::binary::Serializer<NPC>::serialize(output, *this)) return false;

    const std::array<std::string, 4> Suffixes{"up.anim", "right.anim", "down.anim", "left.anim"};
    const std::string ap =
        bl::util::FileUtil::joinPath(Properties::CharacterAnimationPath(), animField);
    for (const std::string& s : Suffixes) {
        const std::string p = bl::util::FileUtil::joinPath(ap, s);
        if (bl::util::FileUtil::exists(p)) { ctx.addDependencyFile(p); }
        else { BL_LOG_WARN << "NPC " << nameField << " is missing anim: " << p; }
    }

    const std::string c =
        bl::util::FileUtil::joinPath(Properties::ConversationPath(), conversationField);
    if (bl::util::FileUtil::exists(c)) { ctx.addDependencyFile(c); }
    else { BL_LOG_WARN << "NPC " << nameField << " is missing conversation " << conversationField; }

    return true;
}

bool NPC::load(const std::string& file, bl::tmap::Direction spawnDir) {
    if (!NpcManager::initializeExisting(file, *this)) return false;
    if (behavior().type() == Behavior::StandStill) { behavior().standing().facedir = spawnDir; }
    return true;
}

bool NPC::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<NPC>::deserializeStream(input, *this);
}

bool NPC::loadProd(bl::serial::binary::InputStream& input) {
    return bl::serial::binary::Serializer<NPC>::deserialize(input, *this);
}

std::string& NPC::name() { return nameField; }

const std::string& NPC::name() const { return nameField; }

std::string& NPC::animation() { return animField; }

const std::string& NPC::animation() const { return animField; }

std::string& NPC::conversation() { return conversationField; }

const std::string& NPC::conversation() const { return conversationField; }

Behavior& NPC::behavior() { return behaviorField; }

Behavior NPC::behavior() const { return behaviorField; }

} // namespace file
} // namespace core
