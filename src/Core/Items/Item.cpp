#include <Core/Items/Item.hpp>

#include <BLIB/Logging.hpp>
#include <Core/Properties.hpp>
#include <unordered_map>

namespace core
{
namespace item
{
namespace
{
std::vector<Id> ids;
const std::string UnknownName        = "<Unknown Item>";
const std::string UnknownDescription = "<This item is not known to Peoplemon>";

} // namespace

std::unordered_map<Id, std::string>* Item::names        = nullptr;
std::unordered_map<Id, std::string>* Item::descriptions = nullptr;
std::unordered_map<Id, int>* Item::values               = nullptr;

Id Item::cast(unsigned int id) {
    const Id result = static_cast<Id>(id);

    if (id >= 1 && id <= 44) return result;
    if (id >= 50 && id <= 62) return result;

    if (id >= 101 && id <= 138) return result;
    if (id >= 200) return result;

    BL_LOG_WARN << "Unknown item id: " << id;
    return Id::Unknown;
}

void Item::setDataSource(file::ItemDB& db) {
    names        = &db.names;
    descriptions = &db.descriptions;
    values       = &db.values;

    ids.reserve(names->size());
    for (const auto& p : *names) { ids.emplace_back(p.first); }
    std::sort(ids.begin(), ids.end());
}

Category Item::getCategory(Id item) {
    const unsigned int id = static_cast<unsigned int>(item);
    if (item == Id::Unknown) return Category::Unknown;
    if (id <= 100) return Category::Regular;
    if (id <= 200) return Category::Key;
    return Category::TM;
}

Type Item::getType(Id item) {
    const unsigned int id = static_cast<unsigned int>(item);
    if (id >= 1 && id <= 4) return Type::TargetPeoplemon;
    if (id >= 5 && id <= 17) return Type::Peopleball;
    if (id >= 18 && id <= 27) return Type::TargetPeoplemon;
    if (id >= 28 && id <= 32) return Type::EvolveStone;
    if (id == 33 || id == 34 || id == 44) return Type::Useless;
    if (id >= 35 && id <= 37) return Type::PlayerModifier;
    if (id >= 38 && id <= 43) return Type::TargetPeoplemon;
    if (id >= 50 && id <= 62) return Type::HoldItem;
    if (id >= 101 && id <= 138) return Type::KeyItem;
    if (id >= 200) return Type::TM;
    return Type::Unknown;
}

const std::string& Item::getName(Id item) {
    const auto it = names->find(item);
    return it == names->end() ? UnknownName : it->second;
}

const std::string& Item::getDescription(Id item) {
    const auto it = descriptions->find(item);
    return it == descriptions->end() ? UnknownDescription : it->second;
}

int Item::getValue(Id item) {
    const auto it = values->find(item);
    return it == values->end() ? 0 : it->second;
}

const std::vector<Id>& Item::validIds() { return ids; }

UseResult Item::useOnPeoplemon(Id) { return {false, "Unimplemented useOnPeoplemon"}; }

UseResult Item::evolvePeoplemon(Id) { return {false, "Unimplemented evolvePeoplemon"}; }

UseResult Item::useOnPlayer(Id) { return {false, "Unimplemented useOnPlayer"}; }

UseResult Item::useKeyItem(Id) { return {false, "Unimplemented useKeyItem"}; }

} // namespace item
} // namespace core
