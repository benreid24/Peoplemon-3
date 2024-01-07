#include <Core/Maps/Spawn.hpp>

namespace core
{
namespace map
{
Spawn::Spawn()
: id(0) {}

Spawn::Spawn(std::uint16_t _id, const bl::tmap::Position& pos)
: id(_id)
, position(pos) {}

} // namespace map
} // namespace core
