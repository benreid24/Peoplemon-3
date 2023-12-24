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
    switch (renderObject.index()) {
    case 2:
        std::get_if<bl::gfx::BatchSlideshow>(&renderObject)->getPlayer().play();
        break;
    case 3:
        (*std::get_if<std::shared_ptr<bl::gfx::Animation2D>>(&renderObject))->getPlayer().play();
        break;
    }
}

} // namespace map
} // namespace core
