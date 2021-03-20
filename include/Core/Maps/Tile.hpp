#ifndef CORE_MAPS_TILE_HPP
#define CORE_MAPS_TILE_HPP

#include <BLIB/Files/Binary.hpp>
#include <BLIB/Media/Graphics.hpp>
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
class Tile : public bl::file::binary::SerializableObject {
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
     * @brief Copies the information from the given Tile
     *
     * @param copy The tile to copy
     * @return Tile& A reference to this tile
     */
    Tile& operator=(const Tile& copy);

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
     * @brief Initializes the tile's graphic components and position
     *
     * @param tileset The tileset to get media from
     * @param position The position to render at globally
     */
    void initialize(Tileset& tileset, const sf::Vector2f& position);

    /**
     * @brief Renders the tile to the given target
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

private:
    bl::file::binary::SerializableField<1, bool> isAnim;
    bl::file::binary::SerializableField<2, IdType> tid;

    sf::Sprite sprite;
    bl::gfx::Animation uniqueAnim;
    bl::gfx::Animation* anim;
    std::function<void(sf::RenderTarget&)> renderFunction;

    void renderSprite(sf::RenderTarget& target) const;
    void renderAnimation(sf::RenderTarget& target) const;

    friend class Tileset;
};

} // namespace map
} // namespace core

#endif
