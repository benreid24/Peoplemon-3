#include <Core/Maps/RenderLevel.hpp>

#include <BLIB/Engine.hpp>

namespace core
{
namespace map
{
void RenderLevel::create(bl::engine::Engine& engine, bl::rc::res::TextureRef ts,
                         unsigned int layerCount, const sf::Vector2u& ms, bl::rc::Scene* s) {
    enginePtr = &engine;
    tileset   = ts;
    scene     = s;
    mapSize   = ms;

    const unsigned int initialCapacity = mapSize.x * mapSize.y / 2;
    zones.reserve(layerCount);
    for (unsigned int i = 0; i < layerCount; ++i) {
        auto& zone = storage.emplace_back();
        zones.emplace_back(&zone);

        zone.tileSprites.create(engine, tileset, initialCapacity);
        zone.tileAnims.create(engine, initialCapacity);

        zone.tileSprites.addToScene(scene, bl::rc::UpdateSpeed::Static);
        zone.tileAnims.addToScene(scene, bl::rc::UpdateSpeed::Static);
    }
}

void RenderLevel::swapLayers(unsigned int i1, unsigned int i2) {
    const bool isGt = i1 > i2;
    const auto it1  = std::next(storage.begin(), i1);
    const auto it2  = std::next(storage.begin(), i2);
    const auto s1   = isGt ? it1 : it2;
    const auto s2   = isGt ? it2 : it1;
    storage.splice(s2, storage, s1);
    std::swap(zones[i1], zones[i2]);
}

void RenderLevel::insertLayer(unsigned int position) {
    auto& layer = *storage.emplace(std::next(storage.begin(), position));
    zones.insert(zones.begin() + position, &layer);

    const unsigned int initialCapacity = mapSize.x * mapSize.y / 2;
    layer.tileSprites.create(*enginePtr, tileset, initialCapacity);
    layer.tileAnims.create(*enginePtr, initialCapacity);
    layer.tileSprites.addToScene(scene, bl::rc::UpdateSpeed::Static);
    layer.tileAnims.addToScene(scene, bl::rc::UpdateSpeed::Static);
}

void RenderLevel::removeLayer(unsigned int position) {
    storage.erase(std::next(storage.begin(), position));
    zones.erase(zones.begin() + position);
}

} // namespace map
} // namespace core
