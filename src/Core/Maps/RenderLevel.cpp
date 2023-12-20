#include <Core/Maps/RenderLevel.hpp>

#include <BLIB/Engine.hpp>

namespace core
{
namespace map
{
void RenderLevel::create(bl::engine::Engine& engine, bl::rc::res::TextureRef tileset,
                         unsigned int layerCount, const sf::Vector2u& mapSize,
                         bl::rc::Scene* scene) {
    for (Zone& zone : zones) {
        const unsigned int initialCapacity = mapSize.x * mapSize.y * layerCount / 2;
        zone.tileSprites.create(engine, tileset, initialCapacity);
        zone.tileAnims.create(engine, initialCapacity);

        zone.tileSprites.addToScene(scene, bl::rc::UpdateSpeed::Static);
        zone.tileAnims.addToScene(scene, bl::rc::UpdateSpeed::Static);
    }
}

RenderLevel::Zone& RenderLevel::getZone(const LayerSet& level, unsigned int layer) {
    if (layer < level.bottomLayers().size()) { return zones[Bottom]; }
    layer -= level.bottomLayers().size();
    if (layer < level.ysortLayers().size()) { return zones[Ysort]; }
    return zones[Top];
}

} // namespace map
} // namespace core
