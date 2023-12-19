#include <Core/Maps/Tile.hpp>
#include <Core/Maps/Tileset.hpp>

#include <iostream>

namespace core
{
namespace map
{
Tile::Tile()
: isAnim(false)
, tid(Blank) {}

Tile::Tile(IdType i, bool a)
: Tile() {
    tid    = i;
    isAnim = a;
}

bool Tile::isAnimation() const { return isAnim; }

Tile::IdType Tile::id() const { return tid; }

void Tile::set(IdType id, bool anim) {
    tid    = id;
    isAnim = anim;
}

void Tile::step() {
    bl::gfx::BatchSlideshow* anim = std::get_if<bl::gfx::BatchSlideshow>(&renderObject);
    if (anim) { anim->getPlayer().play(); }
}

} // namespace map
} // namespace core
