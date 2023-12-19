#include <Core/Maps/Tileset.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace map
{
Tileset::Tileset()
: nextTextureId(1)
, nextAnimationId(1)
, enginePtr(nullptr) {}

Tile::IdType Tileset::addTexture(const std::string& uri) {
    textureFiles.emplace(nextTextureId, uri);
    const auto it = textures
                        .emplace(nextTextureId,
                                 ImageManager::load(
                                     bl::util::FileUtil::joinPath(Properties::MapTilePath(), uri)))
                        .first;
    if (enginePtr) {
        textureAtlas.try_emplace(nextTextureId, textureStitcher.value().addImage(*it->second));
        combinedTextures->ensureSize({textureStitcher.value().getStitchedImage().getSize().x,
                                      textureStitcher.value().getStitchedImage().getSize().y});
        combinedTextures->update(textureStitcher.value().getStitchedImage());
    }
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
    if (it->second->isLooping() && enginePtr) {
        sharedAnimations.try_emplace(nextAnimationId)
            .first->second.create(
                *enginePtr, it->second, bl::gfx::DiscreteAnimation2DPlayer::Auto, true, true);
    }
    return nextAnimationId++;
}

void Tileset::removeAnimation(Tile::IdType id) {
    animFiles.erase(id);
    anims.erase(id);
    sharedAnimations.erase(id);
}

void Tileset::activate(bl::engine::Engine& engine) {
    if (enginePtr != nullptr) { return; }
    enginePtr = &engine;

    for (const auto& apair : anims) {
        if (apair.second->isLooping()) {
            sharedAnimations.try_emplace(apair.first)
                .first->second.create(
                    engine, apair.second, bl::gfx::DiscreteAnimation2DPlayer::Auto, true, true);
        }
    }

    textureStitcher.emplace(engine.renderer());
    for (const auto& tpair : textures) {
        textureAtlas.try_emplace(tpair.first, textureStitcher.value().addImage(*tpair.second));
    }
    combinedTextures =
        engine.renderer().texturePool().createTexture(textureStitcher.value().getStitchedImage());
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
                         ImageManager::load(bl::util::FileUtil::joinPath(Properties::MapTilePath(),
                                                                         tpair.second)));
        if (tpair.first >= nextTextureId) nextTextureId = tpair.first + 1;
    }

    for (const auto& apair : animFiles) {
        auto it = anims
                      .emplace(apair.first,
                               AnimationManager::load(bl::util::FileUtil::joinPath(
                                   Properties::MapAnimationPath(), apair.second)))
                      .first;
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

bl::resource::Ref<sf::Image> Tileset::getTile(Tile::IdType id) const {
    auto it = textures.find(id);
    return it != textures.end() ? it->second : bl::resource::Ref<sf::Image>{};
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

bl::resource::Ref<bl::gfx::a2d::AnimationData> Tileset::getAnim(Tile::IdType id) const {
    auto it = anims.find(id);
    return it != anims.end() ? it->second : bl::resource::Ref<bl::gfx::a2d::AnimationData>{};
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

sf::FloatRect Tileset::getTileTextureBounds(Tile::IdType tid) const {
    const auto tit = textures.find(tid);
    if (tit == textures.end()) {
        BL_LOG_ERROR << "Unable to find texture with id: " << tid;
        return {-1.f, -1.f, -1.f, -1.f};
    }

    const auto ait = textureAtlas.find(tid);
    if (ait == textureAtlas.end()) {
        BL_LOG_ERROR << "Texture with id " << tid << " is missing from atlas";
        return {-1.f, -1.f, -1.f, -1.f};
    }

    const glm::vec2 pos(ait->second);
    const glm::vec2 size(tit->second->getSize().x, tit->second->getSize().y);
    const glm::vec2 posn = combinedTextures->normalizeAndConvertCoord(pos);
    const glm::vec2 br   = combinedTextures->normalizeAndConvertCoord(pos + size);
    return {posn.x, posn.y, br.x - posn.x, br.y - posn.y};
}

} // namespace map
} // namespace core
