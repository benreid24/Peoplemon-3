#ifndef CORE_MAPS_TILE_HPP
#define CORE_MAPS_TILE_HPP

#include <BLIB/Media/Graphics.hpp>
#include <BLIB/Serialization/Binary.hpp>
#include <SFML/Graphics.hpp>
#include <functional>

namespace core
{
namespace map
{
class Tileset;

/**
 * @brief Data representation of a tile in a Map TileLayer
 *
 * @ingroup Maps
 *
 */
class Tile {
public:
    // The type of tile ids in Tileset
    using IdType = std::uint16_t;

    /// Special id for blank tiles
    static constexpr IdType Blank = 0;

    /**
     * @brief Makes a blank tile
     *
     */
    Tile();

    /**
     * @brief Copy constructs from the given tile
     *
     * @param copy The tile to copy
     */
    Tile(const Tile& copy);

    /**
     * @brief Builds a tile with the given info
     *
     * @param id The id of the tile in the tileset
     * @param isAnim True if the tile is an animation, false if it is an image
     */
    Tile(IdType id, bool isAnim = false);

    /**
     * @brief Returns whether this tile is an animation or not
     *
     * @return True if an animation, false if a still image
     */
    bool isAnimation() const;

    /**
     * @brief Returns the id of the image or animation of this tile
     *
     * @return IdType The id of the tile in the tileset
     */
    IdType id() const;

    /**
     * @brief Sets the information of the tile
     *
     * @param tileset Reference to the Tileset being used
     * @param id The id of the image or animation in the Tileset
     * @param anim True if an animation, false if an image
     */
    void set(Tileset& tileset, IdType id, bool anim);

    /**
     * @brief Sets the information of the tile without updating it's graphics
     *
     * @param id The id of the image or animation in the Tileset
     * @param anim True if an animation, false if an image
     */
    void setDataOnly(IdType id, bool anim);

    /**
     * @brief Initializes the tile's graphic components and position
     *
     * @param tileset The tileset to get media from
     * @param position The position to render at globally
     */
    void initialize(Tileset& tileset, const sf::Vector2f& position);

    /**
     * @brief Triggers the animation when the tile is stepped on
     *
     */
    void step();

    /**
     * @brief Updates the tile's unique animation if there is one
     *
     * @param dt Time elapsed since last call to update()
     */
    void update(float dt);

    /**
     * @brief Renders the tile to the given target
     *
     * @param target The target to render to
     * @param lag Residual time between calls to update
     */
    void render(sf::RenderTarget& target, float residual) const;

private:
    bool isAnim;
    IdType tid;

    sf::Sprite sprite;
    bl::gfx::Animation uniqueAnim;
    bl::gfx::Animation* anim;

    friend class Tileset;
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
template<>
struct Serializer<core::map::Tile, false> {
    static bool serialize(OutputStream& output, const core::map::Tile& tile) {
        if (!Serializer<core::map::Tile::IdType>::serialize(output, tile.id())) return false;
        return Serializer<bool>::serialize(output, tile.isAnimation());
    }

    static bool deserialize(InputStream& input, core::map::Tile& result) {
        core::map::Tile::IdType id;
        bool isAnim;
        if (!Serializer<core::map::Tile::IdType>::deserialize(input, id)) return false;
        if (!Serializer<bool>::deserialize(input, isAnim)) return false;
        result.setDataOnly(id, isAnim);
        return true;
    }

    static std::uint32_t size(const core::map::Tile&) {
        return Serializer<core::map::Tile::IdType>::size(core::map::Tile::Blank) +
               Serializer<bool>::size(false);
    }
};
} // namespace binary
} // namespace file
} // namespace bl

#endif
