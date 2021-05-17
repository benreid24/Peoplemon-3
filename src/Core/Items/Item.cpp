#include <Core/Items/Item.hpp>

#include <BLIB/Logging.hpp>

namespace core
{
namespace item
{
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

UseResult Item::useOnPeoplemon(Id item) { return {false, "Unimplemented useOnPeoplemon"}; }

UseResult Item::evolvePeoplemon(Id item) { return {false, "Unimplemented evolvePeoplemon"}; }

UseResult Item::useOnPlayer(Id item) { return {false, "Unimplemented useOnPlayer"}; }

UseResult Item::useKeyItem(Id item) { return {false, "Unimplemented useKeyItem"}; }

} // namespace item
} // namespace core
