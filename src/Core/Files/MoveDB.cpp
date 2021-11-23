#include <Core/Files/MoveDB.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace file
{
using namespace pplmn;

struct MoveDBLoader : public bl::file::binary::VersionedPayloadLoader<MoveDB> {
    virtual bool read(MoveDB& v, bl::file::binary::File& input) const override {
        return v.deserialize(input);
    }

    virtual bool write(const MoveDB& v, bl::file::binary::File& output) const override {
        return v.serialize(output);
    }
};

namespace
{
struct LegacyDBLoader : public bl::file::binary::VersionedPayloadLoader<MoveDB> {
    virtual bool read(MoveDB& db, bl::file::binary::File& input) const override {
        std::uint16_t n;
        if (!input.read<std::uint16_t>(n)) return false;
        for (std::uint16_t i = 0; i < n; ++i) {
            std::uint16_t u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            const MoveId id = static_cast<MoveId>(u16);

            std::string str;
            if (!input.read(str)) return false;
            db.names()[id]          = str;
            db.animationPaths()[id] = str;
            if (!input.read(str)) return false;
            db.descriptions()[id] = str;

            std::uint8_t u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.specials()[id] = u8 != 0;

            if (!input.read<std::uint16_t>(u16)) return false;
            db.damages()[id] = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            db.accuracies()[id] = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            db.priorities()[id] = u16;
            if (!input.read<std::uint16_t>(u16)) return false;
            db.pps()[id] = u16;

            if (!input.read<std::uint8_t>(u8)) return false;
            db.types()[id] = legacyTypeToNew(u8);

            if (!input.read<std::uint8_t>(u8)) return false;
            db.effects()[id] = static_cast<MoveEffect>(u8);
            if (!input.read<std::uint8_t>(u8)) return false;
            db.effectChances()[id] = u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.effectIntensities()[id] = u8;
            if (!input.read<std::uint8_t>(u8)) return false;
            db.effectSelves()[id] = u8 != 0;

            // unsued data from legacy format
            if (!input.read(str)) return false;
            if (!input.read(str)) return false;
            if (!input.read<std::uint8_t>(u8)) return false;
            if (!input.read<std::uint8_t>(u8)) return false;
        }
        return true;
    }

    virtual bool write(const MoveDB&, bl::file::binary::File&) const override { return false; }
};

using VersionedLoader = bl::file::binary::VersionedFile<MoveDB, LegacyDBLoader, MoveDBLoader>;

} // namespace

MoveDB::MoveDB()
: names(*this)
, descriptions(*this)
, animationPaths(*this)
, types(*this)
, damages(*this)
, accuracies(*this)
, priorities(*this)
, pps(*this)
, contactors(*this)
, specials(*this)
, effects(*this)
, effectChances(*this)
, effectIntensities(*this)
, effectSelves(*this) {}

bool MoveDB::load() {
    bl::file::binary::File input(Properties::MoveDBFile(), bl::file::binary::File::Read);
    VersionedLoader loader;
    return loader.read(input, *this);
}

bool MoveDB::save() const {
    bl::file::binary::File input(Properties::MoveDBFile(), bl::file::binary::File::Write);
    VersionedLoader loader;
    return loader.write(input, *this);
}

} // namespace file
} // namespace core
