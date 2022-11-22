#ifndef CORE_MAPS_TILESET_HPP
#define CORE_MAPS_TILESET_HPP

#include <Core/Maps/Tile.hpp>

#include <BLIB/Media/Graphics.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Serialization.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <unordered_map>

namespace core
{
namespace map
{
/// Collection of classes to handle loading legacy and new formats
namespace loaders
{
/**
 * @brief Loads the old Peoplemon format tilesets
 *
 * @ingroup Maps
 *
 */
class LegacyTilesetLoader;

/**
 * @brief Loads the new tileset format
 *
 * @ingroup Maps
 *
 */
class PrimaryTilesetLoader;

} // namespace loaders

/**
 * @brief Stores the collection of images and animations used by Tiles in a Map
 *
 * @ingroup Maps
 *
 */
class Tileset {
public:
    using TileStore = std::unordered_map<Tile::IdType, bl::resource::Resource<sf::Texture>::Ref>;
    using AnimStore =
        std::unordered_map<Tile::IdType, bl::resource::Resource<bl::gfx::AnimationData>::Ref>;

    /**
     * @brief Creates an empty Tileset
     *
     */
    Tileset();

    /**
     * @brief Adds a texture to the tileset and returns its id
     *
     * @param uri The path of the texture to add
     * @return Tile::IdType The id of the new texture
     */
    Tile::IdType addTexture(const std::string& uri);

    /**
     * @brief Removes the given texture from the tileset. Undefined behavior if any tiles reference
     *        it. Care must be taken to blank those tiles before the next render
     *
     * @param id The id of the texture to remove
     */
    void removeTexture(Tile::IdType id);

    /**
     * @brief Adds the given animation to the tileset and returns its id
     *
     * @param uri The path to the animation
     * @return Tile::IdType The id of the new animation
     */
    Tile::IdType addAnimation(const std::string& uri);

    /**
     * @brief Removes the given animation from the tileset. Undefined behavior if any tiles
     *        reference it. Care must be taken to blank those tiles before the next render
     *
     * @param id The id of the animation to remove
     */
    void removeAnimation(Tile::IdType id);

    /**
     * @brief Returns the height of the requested tile, in pixels
     *
     * @param id The id of the tile
     * @param isAnim True for animation, false for sprite
     * @return unsigned int The height of the tile in pixels, or 0 if not found
     */
    unsigned int tileHeight(Tile::IdType id, bool isAnim) const;

    /**
     * @brief Loads the tileset from the given file and loads all media
     *
     * @param file The file to load from
     * @return True if loaded successfully, false on error
     */
    bool load(const std::string& file);

    /**
     * @brief Saves the tileset to the given file. No media is saved
     *
     * @param file The file to save to
     * @return True if successfully written, false on error
     */
    bool save(const std::string& file) const;

    /**
     * @brief Starts playing all shared animations
     *
     */
    void activate();

    /**
     * @brief Updates the animations in the tileset. Does not update unique animations, only shared.
     *        Unique animations are updated by the Tile that renders them
     *
     * @param dt Time elapsed since last called to update() in seconds
     */
    void update(float dt);

    /**
     * @brief Returns a tile from the set. Returns nullptr if not found
     *
     * @param id The id of the tile to get
     * @return bl::resource::Resource<sf::Texture>::Ref A reference to the tile
     */
    bl::resource::Resource<sf::Texture>::Ref getTile(Tile::IdType id);

    /**
     * @brief Returns all contained tiles
     *
     */
    std::vector<TileStore::const_iterator> getTiles() const;

    /**
     * @brief Returns an animation from the set. Returns nullptr if not found
     *
     * @param id The id of the animation to get
     * @return bl::resource::Resource<sf::Texture>::Ref A reference to the animation
     */
    bl::resource::Resource<bl::gfx::AnimationData>::Ref getAnim(Tile::IdType id);

    /**
     * @brief Returns all contained animations
     *
     */
    std::vector<AnimStore::const_iterator> getAnims() const;

private:
    std::unordered_map<Tile::IdType, std::string> textureFiles;
    std::unordered_map<Tile::IdType, std::string> animFiles;
    Tile::IdType nextTextureId;
    Tile::IdType nextAnimationId;

    TileStore textures;
    AnimStore anims;
    std::unordered_map<Tile::IdType, bl::gfx::Animation> sharedAnimations;

    void initializeTile(Tile& tile);

    friend class Tile;
    friend class loaders::LegacyTilesetLoader;
    friend class loaders::PrimaryTilesetLoader;
    friend struct bl::serial::SerializableObject<Tileset>;
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::Tileset> : public SerializableObjectBase {
    using TS = core::map::Tileset;
    using T  = core::map::Tile;

    SerializableField<1, TS, std::unordered_map<T::IdType, std::string>> textureFiles;
    SerializableField<2, TS, std::unordered_map<T::IdType, std::string>> animFiles;

    SerializableObject()
    : textureFiles("textures", *this, &TS::textureFiles, SerializableFieldBase::Required{})
    , animFiles("anims", *this, &TS::animFiles, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
