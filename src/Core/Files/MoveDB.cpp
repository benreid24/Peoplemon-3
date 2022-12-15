#include <Core/Files/MoveDB.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{
using namespace pplmn;

bool MoveDB::load() { return MoveDbManager::initializeExisting(Properties::MoveDBFile(), *this); }

bool MoveDB::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<MoveDB>::deserializeStream(input, *this);
}

bool MoveDB::loadProd(bl::serial::binary::InputStream& input) {
    return bl::serial::binary::Serializer<MoveDB>::deserialize(input, *this);
}

bool MoveDB::save() const {
    std::ofstream output(Properties::MoveDBFile().c_str());
    return bl::serial::json::Serializer<MoveDB>::serializeStream(output, *this, 4, 0);
}

bool MoveDB::saveBundle(bl::serial::binary::OutputStream& output,
                        bl::resource::bundle::FileHandlerContext&) const {
    return bl::serial::binary::Serializer<MoveDB>::serialize(output, *this);
}

} // namespace file
} // namespace core
