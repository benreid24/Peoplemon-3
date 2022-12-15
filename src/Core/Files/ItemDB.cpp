#include <Core/Files/ItemDB.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{

bool ItemDB::load() {
    return ItemDbManager::initializeExisting(Properties::ItemMetadataFile(), *this);
}

bool ItemDB::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<ItemDB>::deserializeStream(input, *this);
}

bool ItemDB::loadProd(bl::serial::binary::InputStream& input) {
    return bl::serial::binary::Serializer<ItemDB>::deserialize(input, *this);
}

bool ItemDB::save() const {
    std::ofstream output(Properties::ItemMetadataFile().c_str());
    return bl::serial::json::Serializer<ItemDB>::serializeStream(output, *this, 4, 0);
}

bool ItemDB::saveBundle(bl::serial::binary::OutputStream& output,
                        bl::resource::bundle::FileHandlerContext&) const {
    return bl::serial::binary::Serializer<ItemDB>::serialize(output, *this);
}

} // namespace file
} // namespace core
