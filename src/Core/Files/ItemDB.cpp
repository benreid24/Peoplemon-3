#include <Core/Files/ItemDB.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

using namespace bl::serial::binary;

namespace core
{
namespace file
{
struct ItemDBLoader : public SerializerVersion<ItemDB> {
    using Serial = Serializer<ItemDB>;

    virtual bool read(ItemDB& db, InputStream& input) const override {
        return Serial::deserialize(input, db);
    }

    virtual bool write(const ItemDB& db, OutputStream& output) const override {
        return Serial::serialize(output, db);
    }
};
namespace
{
struct LegacyDBLoader : public SerializerVersion<ItemDB> {
    virtual bool read(ItemDB& db, InputStream& input) const override {
        std::uint16_t n;
        if (!input.read<std::uint16_t>(n)) return false;
        for (std::uint16_t i = 0; i < n; ++i) {
            std::uint16_t rawid;
            if (!input.read<std::uint16_t>(rawid)) return false;
            const item::Id id = static_cast<item::Id>(rawid);

            std::string str;
            if (!input.read(str)) return false;
            db.names[id] = str;
            if (!input.read(str)) return false;
            db.descriptions[id] = str;

            std::uint32_t val;
            if (!input.read<std::uint32_t>(val)) return false;
            db.values[id] = val;
        }
        return true;
    }

    virtual bool write(const ItemDB&, OutputStream&) const override { return false; }
};

using VersionedLoader = VersionedSerializer<ItemDB, LegacyDBLoader, ItemDBLoader>;

} // namespace

bool ItemDB::load() {
    return ItemDbManager::initializeExisting(Properties::ItemMetadataFile(), *this);
}

bool ItemDB::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<ItemDB>::deserializeStream(input, *this);
}

bool ItemDB::loadProd(bl::serial::binary::InputStream& input) {
    return VersionedLoader::read(input, *this);
}

bool ItemDB::save() const {
    std::ofstream output(Properties::ItemMetadataFile().c_str());
    return bl::serial::json::Serializer<ItemDB>::serializeStream(output, *this, 4, 0);
}

} // namespace file
} // namespace core
