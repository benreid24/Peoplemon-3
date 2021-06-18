#include <Core/Maps/Tile.hpp>
#include <Core/Maps/Tileset.hpp>

#include <iostream>

namespace core
{
namespace map
{
Tile::Tile()
: isAnim(false)
, tid(Blank)
, anim(nullptr) {}

Tile::Tile(const Tile& copy)
: Tile() {
    *this = copy;
}

Tile::Tile(IdType i, bool a)
: Tile() {
    tid    = i;
    isAnim = a;
}

Tile& Tile::operator=(const Tile& copy) {
    isAnim = copy.isAnim;
    tid    = copy.tid;
    return *this;
}

bool Tile::isAnimation() const { return isAnim; }

Tile::IdType Tile::id() const { return tid; }

void Tile::setDataOnly(IdType id, bool anim) {
    tid    = id;
    isAnim = anim;
}

void Tile::set(Tileset& tileset, IdType id, bool anim) {
    tid    = id;
    isAnim = anim;
    initialize(tileset, sprite.getPosition());
}

void Tile::initialize(Tileset& tileset, const sf::Vector2f& pos) {
    tileset.initializeTile(*this);
    sprite.setPosition(pos);
    uniqueAnim.setPosition(pos);

    if (tid == Blank) return;

    if (isAnim) { anim->setIsCentered(false); }
    else {
        anim = nullptr;
    }
}

void Tile::update(float dt) {
    if (anim == &uniqueAnim) uniqueAnim.update(dt);
}

void Tile::render(sf::RenderTarget& target, float lag) const {
    if (tid != Blank) {
        if (isAnim) {
            anim->setPosition(sprite.getPosition()); // in case of shared anim
            anim->render(target, lag);
        }
        else {
            target.draw(sprite);
        }
    }
}

} // namespace map
} // namespace core
