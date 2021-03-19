#ifndef CORE_MAPS_TILE_HPP
#define CORE_MAPS_TILE_HPP

#include <BLIB/Files/Binary.hpp>

namespace core
{
namespace map
{
class Tile : public bl::file::binary::SerializableObject {
public:
    Tile();

    Tile(const Tile& copy);

    Tile& operator=(const Tile& copy);

    bool isAnimation() const;

    std::uint16_t id() const;

    void set(std::uint16_t id, bool anim);

private:
    bl::file::binary::SerializableField<1, bool> isAnim;
    bl::file::binary::SerializableField<2, std::uint16_t> tid;
};

} // namespace map
} // namespace core

#endif
