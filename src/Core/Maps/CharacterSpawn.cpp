#include <Core/Maps/CharacterSpawn.hpp>

namespace core
{
namespace map
{
CharacterSpawn::CharacterSpawn()
: position(*this)
, file(*this) {}

CharacterSpawn::CharacterSpawn(const component::Position& pos, const std::string& f)
: CharacterSpawn() {
    position = pos;
    file     = f;
}

CharacterSpawn::CharacterSpawn(const CharacterSpawn& copy)
: CharacterSpawn() {
    *this = copy;
}

CharacterSpawn& CharacterSpawn::operator=(const CharacterSpawn& copy) {
    position = copy.position;
    file     = copy.file;
    return *this;
}

} // namespace map
} // namespace core
