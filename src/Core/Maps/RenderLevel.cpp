#include <Core/Maps/RenderLevel.hpp>

#include <BLIB/Engine.hpp>

namespace core
{
namespace map
{
void RenderLevel::create(bl::engine::Engine& engine, bl::rc::res::TextureRef tileset,
                         unsigned int layerCount, const sf::Vector2u& mapSize,
                         bl::rc::Scene* scene) {
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

} // namespace map
} // namespace core
