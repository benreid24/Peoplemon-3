#include <Core/Maps/Tile.hpp>
#include <Core/Maps/Tileset.hpp>

#include <iostream>

namespace core
{
namespace map
{
Tile::Tile()
: isAnim(*this, false)
, tid(*this, Blank)
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
    isAnim = copy.isAnim.getValue();
    tid    = copy.tid.getValue();
    return *this;
}

bool Tile::isAnimation() const { return isAnim; }

Tile::IdType Tile::id() const { return tid; }

void Tile::set(Tileset& tileset, IdType id, bool anim) {
    tid    = id;
    isAnim = anim;
    initialize(tileset, sprite.getPosition());
}

void Tile::initialize(Tileset& tileset, const sf::Vector2f& pos) {
    tileset.initializeTile(*this);
    sprite.setPosition(pos);
    uniqueAnim.setPosition(pos);

    static const auto blank    = [](sf::RenderTarget&) {};
    static const auto noUpdate = [](float) {};

    renderFunction = blank;
    updateFunction = noUpdate;

    if (tid.getValue() == Blank) return;

    if (isAnim) {
        if (anim == &uniqueAnim) {
            updateFunction = std::bind(&Tile::update, this, std::placeholders::_1);
        }
        renderFunction = std::bind(&Tile::renderAnimation, this, std::placeholders::_1);
    }
    else {
        anim           = nullptr;
        renderFunction = std::bind(&Tile::renderSprite, this, std::placeholders::_1);
    }
}

void Tile::update(float dt) { updateFunction(dt); }

void Tile::render(sf::RenderTarget& target, float dt) const {
    updateFunction(dt);
    renderFunction(target);
}

void Tile::renderSprite(sf::RenderTarget& target) const { target.draw(sprite); }

void Tile::renderAnimation(sf::RenderTarget& target) const {
    anim->setPosition(sprite.getPosition()); // in case of shared anim
    target.draw(*anim);
}

} // namespace map
} // namespace core
