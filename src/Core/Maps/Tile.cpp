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

    renderFunction = &Tile::noRender;
    updateFunction = &Tile::noUpdate;

    if (tid == Blank) return;

    if (isAnim) {
        anim->setIsCentered(false);
        if (anim == &uniqueAnim) { updateFunction = &Tile::doUpdate; }
        renderFunction = &Tile::renderAnimation;
    }
    else {
        anim           = nullptr;
        renderFunction = &Tile::renderSprite;
    }
}

void Tile::update(float dt) {
    Tile& me = *this;
    (me.*(me.updateFunction))(dt);
}

void Tile::doUpdate(float dt) { uniqueAnim.update(dt); }

void Tile::render(sf::RenderTarget& target, float lag) const {
    const Tile& me = *this;
    (me.*(me.renderFunction))(target, lag);
}

void Tile::renderSprite(sf::RenderTarget& target, float) const { target.draw(sprite); }

void Tile::renderAnimation(sf::RenderTarget& target, float lag) const {
    // TODO - fix legacy maps on conversion so anims are not offset
    anim->setPosition(sprite.getPosition() - sf::Vector2f(32, 32)); // in case of shared anim
    anim->render(target, lag);
}

} // namespace map
} // namespace core
