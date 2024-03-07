#include <Game/Menus/StorageGrid.hpp>

#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Resources.hpp>
#include <Game/Menus/StorageCursor.hpp>

namespace game
{
namespace menu
{
StorageGrid::StorageGrid(bl::engine::Engine& engine)
: engine(engine) {
    background.create(engine);
    background.getOverlayScaler().setFixedScissor(
        sf::IntRect(BoxPosition.x, BoxPosition.y, BoxSize.x, BoxSize.y));
    background.getTransform().setPosition(BoxPosition);
}

void StorageGrid::activate(bl::ecs::Entity parent) { background.setParent(parent); }

void StorageGrid::deactivate() {
    for (auto& ppl : peoplemon) { ppl.removeFromScene(); }
    peoplemon.clear();
}

void StorageGrid::notifyOffset(float offset) {
    background.getTransform().setPosition(BoxPosition.x + offset, BoxPosition.y);
}

void StorageGrid::update(const std::vector<core::pplmn::StoredPeoplemon>& box) {
    deactivate();

    for (const auto& ppl : box) {
        auto& rp = peoplemon.emplace_back();
        rp.create(engine,
                  engine.renderer().texturePool().getOrLoadTexture(
                      core::pplmn::Peoplemon::thumbnailImage(ppl.peoplemon.id())));
        rp.scaleToSize({StorageCursor::TileSize(), StorageCursor::TileSize()});
        rp.getTransform().setPosition(glm::vec2(ppl.position.x, ppl.position.y) *
                                      StorageCursor::TileSize());
        rp.setParent(background);
        rp.addToScene(engine.renderer().getObserver().getCurrentOverlay(),
                      bl::rc::UpdateSpeed::Static);
    }
}

} // namespace menu
} // namespace game
