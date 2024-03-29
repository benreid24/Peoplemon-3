#include <Game/Menus/StorageGrid.hpp>

#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Resources.hpp>
#include <Game/Menus/StorageCursor.hpp>

namespace game
{
namespace menu
{
void StorageGrid::update(const std::vector<core::pplmn::StoredPeoplemon>& box) {
    peoplemon.clear();
    peoplemon.reserve(box.size());

    for (const auto& ppl : box) {
        auto& rp = peoplemon.emplace_back();
        /* rp.texture =
            TextureManager::load(core::pplmn::Peoplemon::thumbnailImage(ppl.peoplemon.id()));
        rp.sprite.setTexture(*rp.texture, true);
        rp.sprite.setScale(StorageCursor::TileSize() / static_cast<float>(rp.texture->getSize().x),
                           StorageCursor::TileSize() / static_cast<float>(rp.texture->getSize().y));
        rp.sprite.setPosition(sf::Vector2f(ppl.position) * StorageCursor::TileSize());
        */
    }
}

void StorageGrid::render(sf::RenderTarget& target, sf::RenderStates states) const {
    // for (const auto& ppl : peoplemon) { target.draw(ppl.sprite, states); }
}

} // namespace menu
} // namespace game
