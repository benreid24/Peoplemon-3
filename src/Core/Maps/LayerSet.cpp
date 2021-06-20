#include <Core/Maps/LayerSet.hpp>

#include <Core/Maps/Tileset.hpp>
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

    const auto sortLayer = [this, &tileset](TileLayer& layer, unsigned int l) {
        for (unsigned int x = 0; x < layer.width(); ++x) {
            for (unsigned int y = 0; y < layer.height(); ++y) {
                if (layer.get(x, y).id() != Tile::Blank) {
                    *getSortedTile(tileset, l, x, y) = &layer.getRef(x, y);
                }
            }
        }
    };

    ysortedLayers.reserve(ysort.getValue().size());
    for (TileLayer& layer : bottomLayers()) { activateLayer(layer); }
    for (unsigned int i = 0; i < ysortLayers().size(); ++i) {
        TileLayer& layer = ysortLayers()[i];
        activateLayer(layer);
        ysortedLayers.emplace_back(layer.width(), layer.height(), nullptr);
        sortLayer(layer, i);
    }
    for (TileLayer& layer : topLayers()) { activateLayer(layer); }
}

CollisionLayer& LayerSet::collisionLayer() { return collisions.getValue(); }

CatchLayer& LayerSet::catchLayer() { return catches.getValue(); }

std::vector<TileLayer>& LayerSet::bottomLayers() { return bottom.getValue(); }

std::vector<TileLayer>& LayerSet::ysortLayers() { return ysort.getValue(); }

std::vector<TileLayer>& LayerSet::topLayers() { return top.getValue(); }

unsigned int LayerSet::layerCount() const {
    return bottom.getValue().size() + ysort.getValue().size() + top.getValue().size();
}

const std::vector<SortedLayer>& LayerSet::renderSortedLayers() const { return ysortedLayers; }

const std::vector<TileLayer>& LayerSet::bottomLayers() const { return bottom.getValue(); }

const std::vector<TileLayer>& LayerSet::ysortLayers() const { return ysort.getValue(); }

const std::vector<TileLayer>& LayerSet::topLayers() const { return top.getValue(); }

void LayerSet::update(const sf::IntRect& area, float dt) {
    static const auto updateLayer = [](TileLayer& layer, const sf::IntRect& area, float dt) {
        const unsigned int xb = area.left + area.left;
        const unsigned int yb = area.top + area.height;
        for (unsigned int x = area.left; x < xb; ++x) {
            for (unsigned int y = area.top; y < yb; ++y) { layer.getRef(x, y).update(dt); }
        }
    };

    for (TileLayer& layer : bottomLayers()) { updateLayer(layer, area, dt); }
    for (TileLayer& layer : ysortLayers()) { updateLayer(layer, area, dt); }
    for (TileLayer& layer : topLayers()) { updateLayer(layer, area, dt); }
}

Tile** LayerSet::getSortedTile(Tileset& tileset, unsigned int l, unsigned int x, unsigned int y) {
    TileLayer& layer    = ysortLayers()[l];
    SortedLayer& sorted = ysortedLayers[l];
    const unsigned int pixelHeight =
        tileset.tileHeight(layer.get(x, y).id(), layer.get(x, y).isAnimation());
    const unsigned int tileHeight = pixelHeight / Properties::PixelsPerTile();
    const unsigned int offset     = tileHeight / 2 + 1;
    const unsigned int ny         = std::min(y + offset, layer.height() - 1);
    return &sorted(x, ny);
}

} // namespace map
} // namespace core
