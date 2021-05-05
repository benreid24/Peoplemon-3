#include <Core/Files/Trainer.hpp>

#include <BLIB/Files/Binary/VersionedFile.hpp>

namespace core
{
namespace file
{
namespace
{
struct LegacyLoader : public bl::file::binary::VersionedPayloadLoader<Trainer> {
    virtual bool read(Trainer& trainer, bl::file::binary::File& input) const override {
        if (!input.read(trainer.name())) return false;
        if (!input.read(trainer.animation())) return false;
        if (!input.read(trainer.prebattleConversation())) return false;
        if (!input.read(trainer.postBattleConversation())) return false;
        if (!input.read(trainer.lostBattleLine())) return false;

        std::string discard;
        if (!input.read(discard)) return false; //  battle music
        if (!input.read(discard)) return false; // battle background

        if (!input.read<std::uint8_t>(trainer.visionRange())) return false;

        std::uint16_t pplCount;
        if (!input.read<std::uint16_t>(pplCount)) return false;
        for (std::uint16_t i = 0; i < pplCount; ++i) {
            // TODO - load peoplemon
            if (!input.read(discard)) return false; // peoplemon file
        }

        std::uint8_t itemCount;
        if (!input.read<std::uint8_t>(itemCount)) return false;
        for (std::uint8_t i = 0; i < itemCount; ++i) {
            std::uint16_t item; // TODO - store items
            if (!input.read<std::uint16_t>(item)) return false;
        }

        std::uint8_t aiType; // TODO - store trainer ai
        if (!input.read<std::uint8_t>(aiType)) return false;

        std::uint8_t behavior;
        if (!input.read<std::uint8_t>(behavior)) return false;
        trainer.behavior() = static_cast<Behavior>(behavior);

        return true;
    }

    virtual bool write(const Trainer&, bl::file::binary::File&) const override {
        // not implemented
        return false;
    }
};

struct PrimaryLoader : public bl::file::binary::VersionedPayloadLoader<Trainer> {
    virtual bool read(Trainer& trainer, bl::file::binary::File& input) const override {
        return trainer.deserialize(input);
    }

    virtual bool write(const Trainer& trainer, bl::file::binary::File& output) const override {
        return trainer.serialize(output);
    }
};

using VersionedLoader = bl::file::binary::VersionedFile<Trainer, LegacyLoader, PrimaryLoader>;

} // namespace

Trainer::Trainer()
: nameField(*this)
, animField(*this)
, preBattleField(*this)
, postBattleField(*this)
, loseBattleLineField(*this)
, rangeField(*this)
, behaviorField(*this) {}

bool Trainer::save(const std::string& file) const {
    bl::file::binary::File output(file, bl::file::binary::File::Write);
    return serialize(output);
}

bool Trainer::load(const std::string& file) {
    bl::file::binary::File input(file, bl::file::binary::File::Read);
    VersionedLoader loader;
    return loader.read(input, *this);
}

std::string& Trainer::name() { return nameField.getValue(); }

const std::string& Trainer::name() const { return nameField.getValue(); }

std::string& Trainer::animation() { return animField.getValue(); }

const std::string& Trainer::animation() const { return animField.getValue(); }

std::string& Trainer::prebattleConversation() { return preBattleField.getValue(); }

const std::string& Trainer::prebattleConversation() const { return preBattleField.getValue(); }

std::string& Trainer::postBattleConversation() { return postBattleField.getValue(); }

const std::string& Trainer::postBattleConversation() const { return postBattleField.getValue(); }

std::string& Trainer::lostBattleLine() { return loseBattleLineField.getValue(); }

const std::string& Trainer::lostBattleLine() const { return loseBattleLineField.getValue(); }

std::uint8_t& Trainer::visionRange() { return rangeField.getValue(); }

std::uint8_t Trainer::visionRange() const { return rangeField.getValue(); }

Behavior& Trainer::behavior() { return behaviorField.getValue(); }

Behavior Trainer::behavior() const { return behaviorField.getValue(); }

} // namespace file
} // namespace core
