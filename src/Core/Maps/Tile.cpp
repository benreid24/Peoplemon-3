#include <Core/Maps/Tile.hpp>

namespace core
{
namespace map
{
Tile::Tile()
: isAnim(*this, false)
, tid(*this, 0) {}

Tile::Tile(const Tile& copy)
: Tile() {
    *this = copy;
}

Tile& Tile::operator=(const Tile& copy) {
    isAnim = copy.isAnim.getValue();
    tid    = copy.tid.getValue();
    return *this;
}

bool Tile::isAnimation() const { return isAnim; }

std::uint16_t Tile::id() const { return tid; }

void Tile::set(std::uint16_t id, bool anim) {
    tid    = id;
    isAnim = anim;
}

} // namespace map
} // namespace core
