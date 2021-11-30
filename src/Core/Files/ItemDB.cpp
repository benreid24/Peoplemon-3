#include <Core/Files/ItemDB.hpp>
#include <Core/Properties.hpp>

using namespace bl::file::binary;

namespace core
{
namespace file
{
struct ItemDBLoader : public VersionedPayloadLoader<ItemDB> {
    virtual bool read(ItemDB& db, File& input) const override { return db.deserialize(input); }

    virtual bool write(const ItemDB& db, File& output) const override {
        return db.serialize(output);
    }
};
namespace
{
struct LegacyDBLoader : public VersionedPayloadLoader<ItemDB> {
    virtual bool read(ItemDB& db, File& input) const override {
        std::uint16_t n;
        if (!input.read<std::uint16_t>(n)) return false;
        for (std::uint16_t i = 0; i < n; ++i) {
            std::uint16_t rawid;
            if (!input.read<std::uint16_t>(rawid)) return false;
            const item::Id id = static_cast<item::Id>(rawid);

            std::string str;
            if (!input.read(str)) return false;
            db.names()[id] = str;
            if (!input.read(str)) return false;
            db.descriptions()[id] = str;

            std::uint32_t val;
            if (!input.read<std::uint32_t>(val)) return false;
            db.values()[id] = val;
        }
        return true;
    }

    virtual bool write(const ItemDB&, File&) const override { return false; }
};

using VersionedLoader = VersionedFile<ItemDB, LegacyDBLoader, ItemDBLoader>;

} // namespace

ItemDB::ItemDB()
: names(*this)
, descriptions(*this)
, values(*this) {}

bool ItemDB::load() {
    VersionedLoader loader;
    File input(Properties::ItemMetadataFile(), File::Read);
    return loader.read(input, *this);
}

bool ItemDB::save() const {
    VersionedLoader loader;
    File output(Properties::ItemMetadataFile(), File::Write);
    return loader.write(output, *this);
}

} // namespace file
} // namespace core
