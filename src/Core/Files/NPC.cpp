#include <Core/Files/NPC.hpp>

#include <Core/Resources.hpp>

namespace core
{
namespace file
{
namespace
{
struct LegacyLoader : public bl::serial::binary::SerializerVersion<NPC> {
    virtual ~LegacyLoader() = default;

    virtual bool read(NPC& npc, bl::serial::binary::InputStream& input) const override {
        if (!input.read(npc.name())) return false;
        if (!input.read(npc.animation())) return false;
        if (!input.read(npc.conversation())) return false;
        return npc.behavior().legacyLoad(input);
    }

    virtual bool write(const NPC&, bl::serial::binary::OutputStream&) const override {
        // unimplemented
        return false;
    }
};

struct PrimaryLoader : public bl::serial::binary::SerializerVersion<NPC> {
    using Serializer = bl::serial::binary::Serializer<NPC>;

    virtual ~PrimaryLoader() = default;

    virtual bool read(NPC& npc, bl::serial::binary::InputStream& input) const override {
        return Serializer::deserialize(input, npc);
    }

    virtual bool write(const NPC& npc, bl::serial::binary::OutputStream& output) const override {
        return Serializer::serialize(output, npc);
    }
};

using VersionedLoader = bl::serial::binary::VersionedSerializer<NPC, LegacyLoader, PrimaryLoader>;

} // namespace

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
    return VersionedLoader::read(input, *this);
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
