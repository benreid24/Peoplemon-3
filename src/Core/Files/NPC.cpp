#include <Core/Files/NPC.hpp>

#include <Core/Resources.hpp>

namespace core
{
namespace file
{
bool NPC::save(const std::string& file) const {
    std::ofstream output(file.c_str());
    return bl::serial::json::Serializer<NPC>::serializeStream(output, *this, 4, 0);
}

bool NPC::load(const std::string& file, component::Direction spawnDir) {
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
