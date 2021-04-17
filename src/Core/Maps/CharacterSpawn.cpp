#include <Core/Maps/CharacterSpawn.hpp>

namespace core
{
namespace map
{
CharacterSpawn::CharacterSpawn()
: position(*this)
, direction(*this)
, file(*this) {}

CharacterSpawn::CharacterSpawn(const sf::Vector2i& pos, entity::Direction dir, const std::string& f)
: CharacterSpawn() {
    position  = pos;
    direction = dir;
    file      = f;
}

CharacterSpawn::CharacterSpawn(const CharacterSpawn& copy)
: CharacterSpawn() {
    *this = copy;
}

CharacterSpawn& CharacterSpawn::operator=(const CharacterSpawn& copy) {
    position  = copy.position;
    direction = copy.direction;
    file      = copy.file;
    return *this;
}

} // namespace map
} // namespace core
