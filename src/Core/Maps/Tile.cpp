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

void Tile::setDataOnly(IdType id, bool a) {
    tid    = id;
    isAnim = a;
}

void Tile::set(Tileset& tileset, IdType id, bool anim) {
    tid    = id;
    isAnim = anim;

    // TODO - BLIB_UPGRADE - tilemap rendering
    // initialize(tileset, sprite.getPosition());
}

void Tile::initialize(Tileset& tileset, const sf::Vector2f& pos) {
    // TODO - BLIB_UPGRADE - tilemap rendering
    tileset.initializeTile(*this);

    /* sprite.setPosition(pos);
    uniqueAnim.setPosition(pos);

    if (tid == Blank || !isAnim) { anim = nullptr; }
    */
}

void Tile::step() {
    // TODO - BLIB_UPGRADE - tilemap rendering
    // if (anim && !anim->playing()) { anim->play(); }
}

void Tile::render(sf::RenderTarget& target, float lag) const {
    // TODO - BLIB_UPGRADE - tilemap rendering
    /*
    if (tid != Blank) {
        if (isAnim) {
            anim->setPosition(sprite.getPosition()); // in case of shared anim
            anim->render(target, lag);
        }
        else {
            target.draw(sprite);
        }
    }
    */
}

} // namespace map
} // namespace core
