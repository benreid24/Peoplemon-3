#include <Core/Maps/LayerSet.hpp>

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

CollisionLayer& LayerSet::collisionLayer() { return collisions.getValue(); }

CatchLayer& LayerSet::catchLayer() { return catches.getValue(); }

std::vector<TileLayer>& LayerSet::bottomLayers() { return bottom.getValue(); }

std::vector<TileLayer>& LayerSet::ysortLayers() { return ysort.getValue(); }

std::vector<TileLayer>& LayerSet::topLayers() { return top.getValue(); }

} // namespace map
} // namespace core
