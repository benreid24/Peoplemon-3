#include <Core/Maps/Item.hpp>

namespace core
{
namespace map
{
Item::Item()
: id(*this)
, mapId(*this)
, position(*this) {}

Item::Item(std::uint16_t i, std::uint16_t mi, const sf::Vector2i& pos)
: Item() {
    id       = i;
    mapId    = mi;
    position = pos;
}

Item::Item(const Item& copy)
: Item() {
    *this = copy;
}

Item& Item::operator=(const Item& copy) {
    id       = copy.id;
    mapId    = copy.mapId;
    position = copy.position;
    return *this;
}

} // namespace map
} // namespace core