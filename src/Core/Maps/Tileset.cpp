#include <Core/Maps/Tileset.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace map
{
Tileset::Tileset()
: nextTextureId(1)
, nextAnimationId(1) {}

Tile::IdType Tileset::addTexture(const std::string& uri) {
    textureFiles.emplace(nextTextureId, uri);
    textures.emplace(
        nextTextureId,
        TextureManager::load(bl::util::FileUtil::joinPath(Properties::MapTilePath(), uri)));
    return nextTextureId++;
}

void Tileset::removeTexture(Tile::IdType id) {
    textureFiles.erase(id);
    textures.erase(id);
}

Tile::IdType Tileset::addAnimation(const std::string& uri) {
    animFiles.emplace(nextAnimationId, uri);
    auto it = anims
                  .emplace(nextAnimationId,
                           AnimationManager::load(
                               bl::util::FileUtil::joinPath(Properties::MapAnimationPath(), uri)))
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

bool Tileset::loadDev(std::istream& input) {
    clear();
    if (!bl::serial::json::Serializer<Tileset>::deserializeStream(input, *this)) return false;
    finishLoad();
    return true;
}

void Tileset::clear() {
    textureFiles.clear();
    animFiles.clear();
    nextTextureId = nextAnimationId = 1;
    textures.clear();
    anims.clear();
    sharedAnimations.clear();
}

bool Tileset::loadProd(bl::serial::binary::InputStream& input) {
    clear();
    if (!bl::serial::binary::Serializer<Tileset>::deserialize(input, *this)) return false;
    finishLoad();
    return true;
}

void Tileset::finishLoad() {
    for (const auto& tpair : textureFiles) {
        textures.emplace(tpair.first,
                         TextureManager::load(bl::util::FileUtil::joinPath(
                             Properties::MapTilePath(), tpair.second)));
        if (tpair.first >= nextTextureId) nextTextureId = tpair.first + 1;
    }

    for (const auto& apair : animFiles) {
        auto it = anims
                      .emplace(apair.first,
                               AnimationManager::load(bl::util::FileUtil::joinPath(
                                   Properties::MapAnimationPath(), apair.second)))
                      .first;
        if (it->second->isLooping()) { sharedAnimations.emplace(apair.first, *it->second); }
        if (apair.first >= nextAnimationId) nextAnimationId = apair.first + 1;
    }
}

bool Tileset::save(const std::string& file) const {
    std::ofstream output(bl::util::FileUtil::startsWithPath(file, Properties::TilesetPath()) ?
                             file.c_str() :
                             bl::util::FileUtil::joinPath(Properties::TilesetPath(), file).c_str());
    return bl::serial::json::Serializer<Tileset>::serializeStream(output, *this, 4, 0);
}

bool Tileset::saveBundle(bl::serial::binary::OutputStream& output,
                         bl::resource::bundle::FileHandlerContext& ctx) const {
    if (!bl::serial::binary::Serializer<Tileset>::serialize(output, *this)) return false;
    for (const auto& tp : textureFiles) {
        const std::string p = bl::util::FileUtil::joinPath(Properties::MapTilePath(), tp.second);
        if (bl::util::FileUtil::exists(p)) { ctx.addDependencyFile(p); }
    }
    for (const auto& ap : animFiles) {
        const std::string p =
            bl::util::FileUtil::joinPath(Properties::MapAnimationPath(), ap.second);
        if (bl::util::FileUtil::exists(p)) { ctx.addDependencyFile(p); }
    }
    return true;
}

bl::resource::Ref<sf::Texture> Tileset::getTile(Tile::IdType id) const {
    auto it = textures.find(id);
    return it != textures.end() ? it->second : bl::resource::Ref<sf::Texture>{};
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

bl::resource::Ref<bl::gfx::AnimationData> Tileset::getAnim(Tile::IdType id) const {
    auto it = anims.find(id);
    return it != anims.end() ? it->second : bl::resource::Ref<bl::gfx::AnimationData>{};
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

std::string Tileset::getFullPath(const std::string& path) {
    return bl::util::FileUtil::joinPath(Properties::TilesetPath(), path);
}

} // namespace map
} // namespace core
