#include <Core/Maps/Spawn.hpp>

namespace core
{
namespace map
{
Spawn::Spawn()
: id(*this)
, position(*this)
, direction(*this) {}

Spawn::Spawn(std::uint16_t _id, const sf::Vector2i& pos, entity::Direction dir)
: Spawn() {
    id        = _id;
    position  = pos;
    direction = dir;
}

Spawn::Spawn(const Spawn& copy)
: Spawn() {
    *this = copy;
}

Spawn& Spawn::operator=(const Spawn& copy) {
    id        = copy.id;
    position  = copy.position;
    direction = copy.direction;
    return *this;
}

} // namespace map
} // namespace core
