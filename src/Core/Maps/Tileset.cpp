#include <BLIB/Engine/Resources.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace map
{
namespace loaders
{
struct LegacyTilesetLoader : public bl::serial::binary::SerializerVersion<Tileset> {
    virtual bool read(Tileset& tileset, bl::serial::binary::InputStream& input) const override {
        std::uint16_t n = 0;

        if (!input.read<std::uint16_t>(n)) return false;
        for (unsigned int i = 0; i < n; ++i) {
            std::string path;
            std::uint16_t id;
            if (!input.read<std::uint16_t>(id)) return false;
            if (!input.read(path)) return false;
            tileset.textureFiles.emplace(id, path);
        }

        if (!input.read<std::uint16_t>(n)) return false;
        for (unsigned int i = 0; i < n; ++i) {
            std::string path;
            std::uint16_t id;
            if (!input.read<std::uint16_t>(id)) return false;
            if (!input.read(path)) return false;
            tileset.animFiles.emplace(id, path);
        }

        return true;
    }

    virtual bool write(const Tileset&, bl::serial::binary::OutputStream&) const override {
        // unimplemented
        return false;
    }
};

struct PrimaryTilesetLoader : public bl::serial::binary::SerializerVersion<Tileset> {
    using Serializer = bl::serial::binary::Serializer<Tileset>;

    virtual bool read(Tileset& tileset, bl::serial::binary::InputStream& input) const override {
        return Serializer::deserialize(input, tileset);
    }

    virtual bool write(const Tileset& tileset,
                       bl::serial::binary::OutputStream& output) const override {
        return Serializer::serialize(output, tileset);
    }
};

} // namespace loaders

namespace
{
using VersionedSerializer =
    bl::serial::binary::VersionedSerializer<Tileset, loaders::LegacyTilesetLoader,
                                            loaders::PrimaryTilesetLoader>;
}

Tileset::Tileset()
: nextTextureId(1)
, nextAnimationId(1) {}

Tile::IdType Tileset::addTexture(const std::string& uri) {
    textureFiles.emplace(nextTextureId, uri);
    textures.emplace(nextTextureId,
                     bl::engine::Resources::textures()
                         .load(bl::util::FileUtil::joinPath(Properties::MapTilePath(), uri))
                         .data);
    return nextTextureId++;
}

void Tileset::removeTexture(Tile::IdType id) {
    textureFiles.erase(id);
    textures.erase(id);
}

Tile::IdType Tileset::addAnimation(const std::string& uri) {
    animFiles.emplace(nextAnimationId, uri);
    auto it =
        anims
            .emplace(nextAnimationId,
                     bl::engine::Resources::animations()
                         .load(bl::util::FileUtil::joinPath(Properties::MapAnimationPath(), uri))
                         .data)
            .first;
    if (it->second->isLooping()) {
        auto jit = sharedAnimations.try_emplace(nextAnimationId, *it->second).first;
        jit->second.play();
    }
    return nextAnimationId++;
}

void Tileset::removeAnimation(Tile::IdType id) {
    animFiles.erase(id);
    anims.erase(id);
    sharedAnimations.erase(id);
}

void Tileset::initializeTile(Tile& tile) {
    if (tile.id() == Tile::Blank) return;

    if (tile.isAnimation()) {
        auto it = anims.find(tile.id());
        if (it != anims.end()) {
            if (it->second->isLooping()) {
                auto jit = sharedAnimations.find(tile.id());
                if (jit != sharedAnimations.end()) { tile.anim = &jit->second; }
                else {
                    BL_LOG_ERROR << "Shared animation not present for looping anim: " << tile.id();
                }
            }
            else {
                tile.uniqueAnim.setData(*it->second);
                tile.anim = &tile.uniqueAnim;
            }
        }
        else {
            BL_LOG_WARN << "Tile with invalid animation id: " << tile.id() << ". Clearing";
            tile.set(*this, Tile::Blank, false);
        }
    }
    else {
        auto it = textures.find(tile.id());
        if (it != textures.end()) { tile.sprite.setTexture(*it->second, true); }
        else {
            BL_LOG_WARN << "Tile with invalid texture id: " << tile.id() << ". Clearing";
            tile.set(*this, Tile::Blank, false);
        }
    }
}

void Tileset::activate() {
    for (auto& ap : sharedAnimations) { ap.second.play(); }
}

unsigned int Tileset::tileHeight(Tile::IdType id, bool isAnim) const {
    if (isAnim) {
        auto anim = anims.find(id);
        if (anim != anims.end()) {
            if (anim->second->frameCount() > 0) return anim->second->getFrameSize(0).y;
        }
    }
    else {
        auto txtr = textures.find(id);
        if (txtr != textures.end()) { return txtr->second->getSize().y; }
    }
    return 0;
}

void Tileset::update(float dt) {
    for (auto& ap : sharedAnimations) { ap.second.update(dt); }
}

bool Tileset::load(const std::string& file) {
    textureFiles.clear();
    animFiles.clear();
    nextTextureId = nextAnimationId = 1;
    textures.clear();
    anims.clear();
    sharedAnimations.clear();

    bl::serial::binary::InputFile input(
        bl::util::FileUtil::joinPath(Properties::TilesetPath(), file));
    if (!VersionedSerializer::read(input, *this)) return false;

    for (const auto& tpair : textureFiles) {
        textures.emplace(
            tpair.first,
            bl::engine::Resources::textures()
                .load(bl::util::FileUtil::joinPath(Properties::MapTilePath(), tpair.second))
                .data);
        if (tpair.first >= nextTextureId) nextTextureId = tpair.first + 1;
    }

    for (const auto& apair : animFiles) {
        auto it = anims
                      .emplace(apair.first,
                               bl::engine::Resources::animations()
                                   .load(bl::util::FileUtil::joinPath(
                                       Properties::MapAnimationPath(), apair.second))
                                   .data)
                      .first;
        if (it->second->isLooping()) { sharedAnimations.emplace(apair.first, *it->second); }
        if (apair.first >= nextAnimationId) nextAnimationId = apair.first + 1;
    }

    return true;
}

bool Tileset::save(const std::string& file) const {
    bl::serial::binary::OutputFile output(
        bl::util::FileUtil::joinPath(Properties::TilesetPath(), file));
    return VersionedSerializer::write(output, *this);
}

bl::resource::Resource<sf::Texture>::Ref Tileset::getTile(Tile::IdType id) {
    auto it = textures.find(id);
    return it != textures.end() ? it->second : nullptr;
}

std::vector<Tileset::TileStore::const_iterator> Tileset::getTiles() const {
    std::vector<TileStore::const_iterator> result;
    result.reserve(textures.size());
    for (Tile::IdType id = 0; id < nextTextureId; ++id) {
        auto it = textures.find(id);
        if (it != textures.end()) { result.emplace_back(it); }
    }
    return result;
}

bl::resource::Resource<bl::gfx::AnimationData>::Ref Tileset::getAnim(Tile::IdType id) {
    auto it = anims.find(id);
    return it != anims.end() ? it->second : nullptr;
}

std::vector<Tileset::AnimStore::const_iterator> Tileset::getAnims() const {
    std::vector<AnimStore::const_iterator> result;
    result.reserve(anims.size());
    for (Tile::IdType id = 0; id < nextAnimationId; ++id) {
        auto it = anims.find(id);
        if (it != anims.end()) { result.emplace_back(it); }
    }
    return result;
}

} // namespace map
} // namespace core
