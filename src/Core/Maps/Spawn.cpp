#include <Core/Maps/Spawn.hpp>

namespace core
{
namespace map
{
Spawn::Spawn()
: id(*this)
, position(*this) {}

Spawn::Spawn(std::uint16_t _id, const entity::Position& pos)
: Spawn() {
    id       = _id;
    position = pos;
}

Spawn::Spawn(const Spawn& copy)
: Spawn() {
    *this = copy;
}

Spawn& Spawn::operator=(const Spawn& copy) {
    id       = copy.id;
    position = copy.position;
    return *this;
}

} // namespace map
} // namespace core
