#include <Core/Files/MoveDB.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace file
{
using namespace pplmn;

struct MoveDBLoader : public bl::serial::binary::SerializerVersion<MoveDB> {
    using Serializer = bl::serial::binary::Serializer<MoveDB>;

    virtual bool read(MoveDB& v, bl::serial::binary::InputStream& input) const override {
        return Serializer::deserialize(input, v);
    }

    virtual bool write(const MoveDB& v, bl::serial::binary::OutputStream& output) const override {
        return Serializer::serialize(output, v);
    }
};

namespace
{
struct LegacyDBLoader : public bl::serial::binary::SerializerVersion<MoveDB> {
    virtual bool read(MoveDB& db, bl::serial::binary::InputStream& input) const override {
        std::uint16_t n;
        if (!input.read<std::uint16_t>(n)) return false;
        for (std::uint16_t i = 0; i < n; ++i) {
            std::uint16_t u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            const MoveId id = static_cast<MoveId>(u16);

            std::string str;
            if (!input.read(str)) return false;
            db.names[id]          = str;
            db.animationPaths[id] = str;
            if (!input.read(str)) return false;
            db.descriptions[id] = str;

            std::uint8_t u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.specials[id] = u8 != 0;

            if (!input.read<std::uint16_t>(u16)) return false;
            db.damages[id] = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            db.accuracies[id] = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            db.priorities[id] = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            db.pps[id] = u16;

            if (!input.read<std::uint8_t>(u8)) return false;
            db.types[id] = legacyTypeToNew(u8);

            if (!input.read<std::uint8_t>(u8)) return false;
            db.effects[id] = static_cast<MoveEffect>(u8);
            if (!input.read<std::uint8_t>(u8)) return false;
            db.effectChances[id] = u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.effectIntensities[id] = u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.effectSelves[id] = u8 != 0;

            // unsued data from legacy format
            if (!input.read(str)) return false;
            if (!input.read(str)) return false;
            if (!input.read<std::uint8_t>(u8)) return false;
            if (!input.read<std::uint8_t>(u8)) return false;
        }
        return true;
    }

    virtual bool write(const MoveDB&, bl::serial::binary::OutputStream&) const override {
        return false;
    }
};

using VersionedLoader =
    bl::serial::binary::VersionedSerializer<MoveDB, LegacyDBLoader, MoveDBLoader>;

} // namespace

bool MoveDB::load() {
    bl::serial::binary::InputFile input(Properties::MoveDBFile());
    return VersionedLoader::read(input, *this);
}

bool MoveDB::save() const {
    bl::serial::binary::OutputFile output(Properties::MoveDBFile());
    return VersionedLoader::write(output, *this);
}

} // namespace file
} // namespace core
