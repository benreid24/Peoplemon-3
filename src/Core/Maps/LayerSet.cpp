#include <Core/Maps/LayerSet.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace map
{
LayerSet::LayerSet()
: collisions(*this)
, catches(*this)
, bottom(*this)
, ysort(*this)
, top(*this) {}

LayerSet::LayerSet(const LayerSet& copy)
: LayerSet() {
    *this = copy;
}

LayerSet& LayerSet::operator=(const LayerSet& copy) {
    collisions = copy.collisions.getValue();
    catches    = copy.catches.getValue();
    bottom     = copy.bottom.getValue();
    ysort      = copy.ysort.getValue();
    top        = copy.top.getValue();
    return *this;
}

void LayerSet::init(unsigned int width, unsigned int height, unsigned int bottomCount,
                    unsigned int ysortLayerCount, unsigned int topLayerCount) {
    collisionLayer().create(width, height, Collision::Open);
    catchLayer().create(width, height, Catch::NoEncounter);

    bottomLayers().resize(bottomCount);
    for (TileLayer& layer : bottomLayers()) { layer.create(width, height, {Tile::Blank}); }

    ysortLayers().resize(ysortLayerCount);
    for (TileLayer& layer : ysortLayers()) { layer.create(width, height, {Tile::Blank}); }

    topLayers().resize(topLayerCount);
    for (TileLayer& layer : topLayers()) { layer.create(width, height, {Tile::Blank}); }
}

void LayerSet::activate(Tileset& tileset) {
    const auto activateLayer = [&tileset](TileLayer& layer) {
        for (unsigned int x = 0; x < layer.width(); ++x) {
            for (unsigned int y = 0; y < layer.height(); ++y) {
                layer.getRef(x, y).initialize(
                    tileset,
                    {static_cast<float>(x * Properties::PixelsPerTile()),
                     static_cast<float>(y * Properties::PixelsPerTile())});
            }
        }
    };
    for (TileLayer& layer : bottomLayers()) { activateLayer(layer); }
    for (TileLayer& layer : ysortLayers()) { activateLayer(layer); }
    for (TileLayer& layer : topLayers()) { activateLayer(layer); }
}

CollisionLayer& LayerSet::collisionLayer() { return collisions.getValue(); }

CatchLayer& LayerSet::catchLayer() { return catches.getValue(); }

std::vector<TileLayer>& LayerSet::bottomLayers() { return bottom.getValue(); }

std::vector<TileLayer>& LayerSet::ysortLayers() { return ysort.getValue(); }

std::vector<TileLayer>& LayerSet::topLayers() { return top.getValue(); }

} // namespace map
} // namespace core
