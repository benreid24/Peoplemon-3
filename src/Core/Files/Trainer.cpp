#include <Core/Files/Trainer.hpp>
#include <Core/Items/Item.hpp>

namespace core
{
namespace file
{
namespace
{
struct LegacyLoader : public bl::serial::binary::SerializerVersion<Trainer> {
    virtual bool read(Trainer& trainer, bl::serial::binary::InputStream& input) const override {
        if (!input.read(trainer.name)) return false;
        if (!input.read(trainer.animation)) return false;
        if (!input.read(trainer.prebattleConversation)) return false;
        if (!input.read(trainer.postBattleConversation)) return false;
        if (!input.read(trainer.lostBattleLine)) return false;

        std::string discard;
        if (!input.read(discard)) return false; //  battle music
        if (!input.read(discard)) return false; // battle background

        if (!input.read<std::uint8_t>(trainer.visionRange)) return false;

        std::uint16_t pplCount;
        if (!input.read<std::uint16_t>(pplCount)) return false;
        trainer.peoplemon.reserve(pplCount);
        for (std::uint16_t i = 0; i < pplCount; ++i) {
            if (!input.read(discard)) return false;
            trainer.peoplemon.emplace_back();
            if (!trainer.peoplemon.back().loadLegacyFile(discard)) {
                BL_LOG_ERROR << "Bad peoplemon file " << discard << " for trainer " << trainer.name;
                return false;
            }
        }

        std::uint8_t itemCount;
        if (!input.read<std::uint8_t>(itemCount)) return false;
        trainer.items.reserve(itemCount);
        for (std::uint8_t i = 0; i < itemCount; ++i) {
            std::uint16_t item;
            if (!input.read<std::uint16_t>(item)) return false;
            const item::Id it = item::Item::cast(item);
            if (it == item::Id::Unknown) {
                BL_LOG_ERROR << "Bad item " << item << " for trainer " << trainer.name;
                return false;
            }
            trainer.items.emplace_back(it);
        }

        std::uint8_t aiType; // TODO - store trainer ai
        if (!input.read<std::uint8_t>(aiType)) return false;

        return trainer.behavior.legacyLoad(input);
    }

    virtual bool write(const Trainer&, bl::serial::binary::OutputStream&) const override {
        // not implemented
        return false;
    }
};

struct PrimaryLoader : public bl::serial::binary::SerializerVersion<Trainer> {
    using Serializer = bl::serial::binary::Serializer<Trainer>;

    virtual bool read(Trainer& trainer, bl::serial::binary::InputStream& input) const override {
        return Serializer::deserialize(input, trainer);
    }

    virtual bool write(const Trainer& trainer,
                       bl::serial::binary::OutputStream& output) const override {
        return Serializer::serialize(output, trainer);
    }
};

using VersionedLoader =
    bl::serial::binary::VersionedSerializer<Trainer, LegacyLoader, PrimaryLoader>;

} // namespace

Trainer::Trainer()
: payout(0) {}

bool Trainer::save(const std::string& file) const {
    bl::serial::binary::OutputFile output(file);
    return VersionedLoader::write(output, *this);
}

bool Trainer::load(const std::string& file, component::Direction spawnDir) {
    bl::serial::binary::InputFile input(file);
    if (VersionedLoader::read(input, *this)) {
        if (behavior.type() == Behavior::StandStill) { behavior.standing().facedir = spawnDir; }
        sourceFile = file;
        if (payout == 0) { payout = 40; }
        return true;
    }
    return false;
}

} // namespace file
} // namespace core
