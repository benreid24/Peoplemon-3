#include <Game/Menus/StorageGrid.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Game/Menus/StorageCursor.hpp>

namespace game
{
namespace menu
{
void StorageGrid::update(const std::vector<core::pplmn::StoredPeoplemon>& box) {
    peoplemon.clear();
    peoplemon.reserve(box.size());

    for (const auto& ppl : box) {
        peoplemon.emplace_back();
        auto& rp   = peoplemon.back();
        rp.texture = bl::engine::Resources::textures()
                         .load(core::pplmn::Peoplemon::thumbnailImage(ppl.peoplemon.id()))
                         .data;
        rp.sprite.setTexture(*rp.texture, true);
        rp.sprite.setScale(StorageCursor::Tilesize() / static_cast<float>(rp.texture->getSize().x),
                           StorageCursor::Tilesize() / static_cast<float>(rp.texture->getSize().y));
        rp.sprite.setPosition(sf::Vector2f(ppl.position) * StorageCursor::Tilesize());
    }
}

void StorageGrid::render(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& ppl : peoplemon) { target.draw(ppl.sprite, states); }
}

} // namespace menu
} // namespace game
