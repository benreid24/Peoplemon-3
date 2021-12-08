#include <Core/Maps/Item.hpp>

namespace core
{
namespace map
{
Item::Item()
: id(0)
, mapId(0)
, level(0)
, visible(false) {}

Item::Item(std::uint16_t i, std::uint16_t mi, const sf::Vector2i& pos, std::uint8_t l, bool v)
: id(i)
, mapId(mi)
, position(pos)
, level(l)
, visible(v) {}

} // namespace map
} // namespace core
