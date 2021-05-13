#include <Core/Components/Direction.hpp>

namespace core
{
namespace component
{
Direction oppositeDirection(Direction dir) {
    const unsigned int i = static_cast<unsigned int>(dir);
    return static_cast<Direction>((i + 2) % 4);
}

} // namespace component
} // namespace core
