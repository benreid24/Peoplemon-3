#include <Core/Maps/LayerSet.hpp>

#include <Core/Maps/Tileset.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace map
{
void LayerSet::init(unsigned int width, unsigned int height, unsigned int bottomCount,
                    unsigned int ysortLayerCount, unsigned int topLayerCount) {
    collisionLayer().create(width, height, Collision::Open);
    catchLayer().create(width, height, 0);

    bottomLayers().resize(bottomCount);
    for (TileLayer& layer : bottomLayers()) { layer.create(width, height, {Tile::Blank}); }

    ysortLayers().resize(ysortLayerCount);
    for (TileLayer& layer : ysortLayers()) { layer.create(width, height, {Tile::Blank}); }

    topLayers().resize(topLayerCount);
    for (TileLayer& layer : topLayers()) { layer.create(width, height, {Tile::Blank}); }
}

CollisionLayer& LayerSet::collisionLayer() { return collisions; }

const CollisionLayer& LayerSet::collisionLayer() const { return collisions; }

CatchLayer& LayerSet::catchLayer() { return catches; }

const CatchLayer& LayerSet::catchLayer() const { return catches; }

std::vector<TileLayer>& LayerSet::bottomLayers() { return bottom; }

std::vector<TileLayer>& LayerSet::ysortLayers() { return ysort; }

std::vector<TileLayer>& LayerSet::topLayers() { return top; }

unsigned int LayerSet::layerCount() const { return bottom.size() + ysort.size() + top.size(); }

const std::vector<TileLayer>& LayerSet::bottomLayers() const { return bottom; }

const std::vector<TileLayer>& LayerSet::ysortLayers() const { return ysort; }

const std::vector<TileLayer>& LayerSet::topLayers() const { return top; }

TileLayer& LayerSet::getLayer(unsigned int layer) {
    if (layer >= layerCount()) {
        BL_LOG_ERROR << "Accessing out of bounds layer: " << layer;
        return bottom.front();
    }

    if (layer < bottom.size()) { return bottom[layer]; }
    layer -= bottom.size();
    if (layer < ysort.size()) { return ysort[layer]; }
    return top[layer - ysort.size()];
}

} // namespace map
} // namespace core
