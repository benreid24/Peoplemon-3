#include <Core/Files/NPC.hpp>

#include <BLIB/Files/Binary/VersionedFile.hpp>

namespace core
{
namespace file
{
namespace
{
struct LegacyLoader : public bl::file::binary::VersionedPayloadLoader<NPC> {
    virtual ~LegacyLoader() = default;

    virtual bool read(NPC& npc, bl::file::binary::File& input) const override {
        if (!input.read(npc.name())) return false;
        if (!input.read(npc.animation())) return false;
        if (!input.read(npc.conversation())) return false;
        std::uint8_t behavior;
        if (!input.read<std::uint8_t>(behavior)) return false;
        npc.behavior() = static_cast<Behavior>(behavior);
        return true;
    }

    virtual bool write(const NPC&, bl::file::binary::File&) const override {
        // unimplemented
        return false;
    }
};

struct PrimaryLoader : public bl::file::binary::VersionedPayloadLoader<NPC> {
    virtual ~PrimaryLoader() = default;

    virtual bool read(NPC& npc, bl::file::binary::File& input) const override {
        return npc.deserialize(input);
    }

    virtual bool write(const NPC& npc, bl::file::binary::File& output) const override {
        return npc.serialize(output);
    }
};

using VersionedLoader = bl::file::binary::VersionedFile<NPC, LegacyLoader, PrimaryLoader>;

} // namespace

NPC::NPC()
: nameField(*this)
, animField(*this)
, conversationField(*this)
, behaviorField(*this) {}

bool NPC::save(const std::string& file) const {
    bl::file::binary::File output(file, bl::file::binary::File::Write);
    return serialize(output);
}

bool NPC::load(const std::string& file) {
    VersionedLoader loader;
    bl::file::binary::File input(file, bl::file::binary::File::Read);
    return loader.read(input, *this);
}

std::string& NPC::name() { return nameField.getValue(); }

const std::string& NPC::name() const { return nameField.getValue(); }

std::string& NPC::animation() { return animField.getValue(); }

const std::string& NPC::animation() const { return animField.getValue(); }

std::string& NPC::conversation() { return conversationField.getValue(); }

const std::string& NPC::conversation() const { return conversationField.getValue(); }

Behavior& NPC::behavior() { return behaviorField.getValue(); }

Behavior NPC::behavior() const { return behaviorField.getValue(); }

} // namespace file
} // namespace core
