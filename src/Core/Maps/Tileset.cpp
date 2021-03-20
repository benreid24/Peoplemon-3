#include <BLIB/Engine/Resources.hpp>
#include <Core/Maps/Tileset.hpp>

namespace core
{
namespace map
{
// TODO - versioned file for loading legacy tilesets
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
    textures.emplace(nextTextureId, bl::engine::Resources::textures().load(uri).data);
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

bool Tileset::load(bl::file::binary::File& input) {
    textureFiles.getValue().clear();
    animFiles.getValue().clear();
    nextTextureId = nextAnimationId = 1;
    textures.clear();
    anims.clear();
    sharedAnimations.clear();

    if (!deserialize(input)) return false;

    for (const auto& tpair : textureFiles.getValue()) {
        textures.emplace(tpair.first, bl::engine::Resources::textures().load(tpair.second).data);
        if (tpair.first >= nextTextureId) nextTextureId = tpair.first + 1;
    }

    for (const auto& apair : animFiles.getValue()) {
        auto it =
            anims.emplace(apair.first, bl::engine::Resources::animations().load(apair.second).data)
                .first;
        if (it->second->isLooping()) { sharedAnimations.try_emplace(apair.first, *it->second); }
        if (apair.first >= nextAnimationId) nextAnimationId = apair.first + 1;
    }

    return true;
}

bool Tileset::save(bl::file::binary::File& output) const { return serialize(output); }

} // namespace map
} // namespace core
