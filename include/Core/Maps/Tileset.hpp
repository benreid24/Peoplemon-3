#ifndef CORE_MAPS_TILESET_HPP
#define CORE_MAPS_TILESET_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Serialization.hpp>
#include <BLIB/Util/ImageStitcher.hpp>
#include <Core/Maps/Tile.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <unordered_map>

namespace core
{
namespace map
{
/**
 * @brief Stores the collection of images and animations used by Tiles in a Map
 *
 * @ingroup Maps
 */
class Tileset {
public:
    using TileStore = std::unordered_map<Tile::IdType, bl::resource::Ref<sf::Image>>;
    using AnimStore =
        std::unordered_map<Tile::IdType, bl::resource::Ref<bl::gfx::a2d::AnimationData>>;

    /**
     * @brief Creates an empty Tileset
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
     * @brief Loads the tileset from the development format data
     *
     * @param input The input stream to load from
     * @return True if loaded successfully, false on error
     */
    bool loadDev(std::istream& input);

    /**
     * @brief Loads the tileset from the production format data
     *
     * @param input The input stream to load from
     * @return True if loaded successfully, false on error
     */
    bool loadProd(bl::serial::binary::InputStream& input);

    /**
     * @brief Saves the tileset to the given file. No media is saved
     *
     * @param file The file to save to
     * @return True if successfully written, false on error
     */
    bool save(const std::string& file) const;

    /**
     * @brief Saves the data from this object to the given bundle and registers dependency files
     *
     * @param output Stream to output to
     * @param ctx Context to register dependencies with
     * @return True if serialization succeeded, false otherwise
     */
    bool saveBundle(bl::serial::binary::OutputStream& output,
                    bl::resource::bundle::FileHandlerContext& ctx) const;

    /**
     * @brief Prepares renderer resources and starts playing all shared animations
     *
     * @param engine The game engine instance
     */
    void activate(bl::engine::Engine& engine);

    /**
     * @brief Returns a tile from the set. Returns nullptr if not found
     *
     * @param id The id of the tile to get
     * @return bl::resource::Ref<sf::Texture> A reference to the tile
     */
    bl::resource::Ref<sf::Image> getTile(Tile::IdType id) const;

    /**
     * @brief Returns all contained tiles
     */
    std::vector<TileStore::const_iterator> getTiles() const;

    /**
     * @brief Returns an animation from the set. Returns nullptr if not found
     *
     * @param id The id of the animation to get
     * @return bl::resource::Ref<sf::Texture> A reference to the animation
     */
    bl::resource::Ref<bl::gfx::a2d::AnimationData> getAnim(Tile::IdType id) const;

    /**
     * @brief Returns all contained animations
     */
    std::vector<AnimStore::const_iterator> getAnims() const;

    /**
     * @brief Generates the full path to the given tileset file
     *
     * @param path The relative tileset path
     * @return std::string The full path to the given tileset
     */
    static std::string getFullPath(const std::string& path);

private:
    std::unordered_map<Tile::IdType, std::string> textureFiles;
    std::unordered_map<Tile::IdType, std::string> animFiles;
    Tile::IdType nextTextureId;
    Tile::IdType nextAnimationId;

    TileStore textures;
    AnimStore anims;
    bl::engine::Engine* enginePtr;
    std::optional<bl::util::ImageStitcher> textureStitcher;
    std::unordered_map<Tile::IdType, glm::u32vec2> textureAtlas;
    bl::rc::res::TextureRef combinedTextures;
    std::unordered_map<Tile::IdType, bl::gfx::DiscreteAnimation2DPlayer> sharedAnimations;

    void initializeTile(Tile& tile);
    void clear();
    void finishLoad();

    friend class Tile;
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
    : SerializableObjectBase("Tileset")
    , textureFiles("textures", *this, &TS::textureFiles, SerializableFieldBase::Required{})
    , animFiles("anims", *this, &TS::animFiles, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
