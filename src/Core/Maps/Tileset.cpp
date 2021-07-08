#include <BLIB/Engine/Resources.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace map
{
namespace loaders
{
struct LegacyTilesetLoader : public bl::file::binary::VersionedPayloadLoader<Tileset> {
    virtual bool read(Tileset& tileset, bl::file::binary::File& input) const override {
        std::uint16_t n = 0;

        if (!input.read<std::uint16_t>(n)) return false;
        for (unsigned int i = 0; i < n; ++i) {
            std::string path;
            std::uint16_t id;
            if (!input.read<std::uint16_t>(id)) return false;
            if (!input.read(path)) return false;
            tileset.textureFiles.getValue().emplace(id, path);
        }

        if (!input.read<std::uint16_t>(n)) return false;
        for (unsigned int i = 0; i < n; ++i) {
            std::string path;
            std::uint16_t id;
            if (!input.read<std::uint16_t>(id)) return false;
            if (!input.read(path)) return false;
            tileset.animFiles.getValue().emplace(id, path);
        }

        return true;
    }

    virtual bool write(const Tileset&, bl::file::binary::File&) const override {
        // unimplemented
        return false;
    }
};

struct PrimaryTilesetLoader : public bl::file::binary::VersionedPayloadLoader<Tileset> {
    virtual bool read(Tileset& tileset, bl::file::binary::File& input) const override {
        return tileset.deserialize(input);
    }

    virtual bool write(const Tileset& tileset, bl::file::binary::File& output) const override {
        return tileset.serialize(output);
    }
};

} // namespace loaders

namespace
{
using VersionedLoader = bl::file::binary::VersionedFile<Tileset, loaders::LegacyTilesetLoader,
                                                        loaders::PrimaryTilesetLoader>;
}

// TODO - figure out if paths need to be prepended
// TODO - ensure spritesheet found for animations brought in

Tileset::Tileset()
: textureFiles(*this)
, animFiles(*this)
, nextTextureId(1)
, nextAnimationId(1) {}

Tileset::Tileset(const Tileset& copy)
: Tileset() {
    *this = copy;
}

Tileset& Tileset::operator=(const Tileset& copy) {
    textureFiles     = copy.textureFiles;
    animFiles        = copy.animFiles;
    nextTextureId    = copy.nextTextureId;
    nextAnimationId  = copy.nextAnimationId;
    textures         = copy.textures;
    anims            = copy.anims;
    sharedAnimations = copy.sharedAnimations;
    return *this;
}

Tile::IdType Tileset::addTexture(const std::string& uri) {
    textureFiles.getValue().emplace(nextTextureId, uri);
    textures.emplace(nextTextureId,
                     bl::engine::Resources::textures()
                         .load(bl::file::Util::joinPath(Properties::MapTilePath(), uri))
                         .data);
    return nextTextureId++;
}

void Tileset::removeTexture(Tile::IdType id) {
    textureFiles.getValue().erase(id);
    textures.erase(id);
}

Tile::IdType Tileset::addAnimation(const std::string& uri) {
    animFiles.getValue().emplace(nextAnimationId, uri);
    auto it =
        anims.emplace(nextAnimationId, bl::engine::Resources::animations().load(uri).data).first;
    if (it->second->isLooping()) {
        auto jit = sharedAnimations.try_emplace(nextAnimationId, *it->second).first;
        jit->second.play();
    }
    return nextAnimationId++;
}

void Tileset::removeAnimation(Tile::IdType id) {
    animFiles.getValue().erase(id);
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
    textureFiles.getValue().clear();
    animFiles.getValue().clear();
    nextTextureId = nextAnimationId = 1;
    textures.clear();
    anims.clear();
    sharedAnimations.clear();

    bl::file::binary::File input(bl::file::Util::joinPath(Properties::TilesetPath(), file),
                                 bl::file::binary::File::Read);
    VersionedLoader loader;
    if (!loader.read(input, *this)) return false;

    for (const auto& tpair : textureFiles.getValue()) {
        textures.emplace(
            tpair.first,
            bl::engine::Resources::textures()
                .load(bl::file::Util::joinPath(Properties::MapTilePath(), tpair.second))
                .data);
        if (tpair.first >= nextTextureId) nextTextureId = tpair.first + 1;
    }

    for (const auto& apair : animFiles.getValue()) {
        auto it = anims
                      .emplace(apair.first,
                               bl::engine::Resources::animations()
                                   .load(bl::file::Util::joinPath(Properties::MapAnimationPath(),
                                                                  apair.second))
                                   .data)
                      .first;
        if (it->second->isLooping()) { sharedAnimations.emplace(apair.first, *it->second); }
        if (apair.first >= nextAnimationId) nextAnimationId = apair.first + 1;
    }

    return true;
}

bool Tileset::save(const std::string& file) const {
    bl::file::binary::File output(bl::file::Util::joinPath(Properties::TilesetPath(), file),
                                  bl::file::binary::File::Write);
    VersionedLoader loader;
    return loader.write(output, *this);
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
