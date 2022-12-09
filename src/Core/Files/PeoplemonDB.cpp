#include <Core/Files/PeoplemonDB.hpp>

#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{
bool PeoplemonDB::load() {
    return PeoplemonDbManager::initializeExisting(Properties::PeoplemonDBFile(), *this);
}

bool PeoplemonDB::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<PeoplemonDB>::deserializeStream(input, *this);
}

bool PeoplemonDB::loadProd(bl::serial::binary::InputStream& input) {
    return bl::serial::binary::Serializer<PeoplemonDB>::deserialize(input, *this);
}

bool PeoplemonDB::save() const {
    std::ofstream output(Properties::PeoplemonDBFile().c_str());
    return bl::serial::json::Serializer<PeoplemonDB>::serializeStream(output, *this, 4, 0);
}

} // namespace file
} // namespace core
