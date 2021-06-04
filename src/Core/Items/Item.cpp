#include <Core/Items/Item.hpp>

#include <BLIB/Files/JSON.hpp>
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
std::unordered_map<Id, std::string> names;
std::unordered_map<Id, std::string> descriptions;
std::unordered_map<Id, int> values;
const std::string unknownName        = "Unknown Item";
const std::string unknownDescription = "This item is not known to Peoplemon";
bool loaded                          = false;

void checkLoad();

} // namespace

Id Item::cast(unsigned int id) {
    const Id result = static_cast<Id>(id);

    if (id >= 1 && id <= 44) return result;
    if (id >= 50 && id <= 62) return result;

    if (id >= 101 && id <= 138) return result;
    if (id >= 200) return result;

    BL_LOG_WARN << "Unknown item id: " << id;
    return Id::Unknown;
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
    checkLoad();
    const auto it = names.find(item);
    return it == names.end() ? unknownName : it->second;
}

const std::string& Item::getDescription(Id item) {
    checkLoad();
    const auto it = descriptions.find(item);
    return it == descriptions.end() ? unknownDescription : it->second;
}

int Item::getValue(Id item) {
    checkLoad();
    const auto it = values.find(item);
    return it == values.end() ? 0 : it->second;
}

const std::vector<Id>& Item::validIds() {
    checkLoad();
    return ids;
}

UseResult Item::useOnPeoplemon(Id item) { return {false, "Unimplemented useOnPeoplemon"}; }

UseResult Item::evolvePeoplemon(Id item) { return {false, "Unimplemented evolvePeoplemon"}; }

UseResult Item::useOnPlayer(Id item) { return {false, "Unimplemented useOnPlayer"}; }

UseResult Item::useKeyItem(Id item) { return {false, "Unimplemented useKeyItem"}; }

namespace
{
void checkLoad() {
    if (loaded) return;
    loaded = true;

    const bl::file::json::Group data = bl::file::json::loadFromFile(Properties::ItemMetadataFile());
    for (const std::string& idStr : data.getFields()) {
        try {
            const unsigned int rawId = std::stol(idStr);
            const item::Id id        = item::Item::cast(rawId);
            if (id == item::Id::Unknown) {
                BL_LOG_ERROR << "Unknown item id: " << rawId;
                continue;
            }
            ids.emplace_back(id);

            const bl::file::json::RGroup ng = data.getGroup(idStr);
            if (!ng.has_value()) {
                BL_LOG_ERROR << "Item id " << idStr << " has invalid json data";
                continue;
            }

            const bl::file::json::Group& g      = ng.value();
            const bl::file::json::String name   = g.getString("name");
            const bl::file::json::String desc   = g.getString("description");
            const bl::file::json::Numeric value = g.getNumeric("value");
            if (!name.has_value()) {
                BL_LOG_WARN << "Item " << idStr << " is missing 'name' field";
            }
            if (!desc.has_value()) {
                BL_LOG_WARN << "Item " << idStr << " is missing 'description' field";
            }
            if (!value.has_value()) {
                BL_LOG_WARN << "Item " << idStr << " is missing 'value' field";
            }

            names.emplace(id, name.value_or(unknownName));
            descriptions.emplace(id, desc.value_or(unknownDescription));
            values.emplace(id, static_cast<int>(value.value_or(0)));
        } catch (...) { BL_LOG_WARN << "Invalid item id: " << idStr; }
    }
}
} // namespace

} // namespace item
} // namespace core
