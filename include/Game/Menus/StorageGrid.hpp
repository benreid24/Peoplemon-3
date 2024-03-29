#ifndef GAME_MENUS_STORAGEGRID_HPP
#define GAME_MENUS_STORAGEGRID_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Peoplemon/StoredPeoplemon.hpp>
#include <functional>
#include <vector>

namespace game
{
namespace menu
{
/**
 * @brief Renderer for the grid of peoplemon in a storage box
 *
 * @ingroup Menus
 */
class StorageGrid {
public:
    // TODO - BLIB_UPGRADE - storage box rendering

    /**
     * @brief Updates the set of peoplemon to render
     *
     * @param box The set of stored peoplemon to render
     */
    void update(const std::vector<core::pplmn::StoredPeoplemon>& box);

    /**
     * @brief Renders the box of peoplemon
     *
     * @param target The target to render to
     * @param states Render states to use
     */
    void render(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    struct Stored {
        bl::rc::res::TextureRef texture;
        bl::gfx::Sprite sprite;
    };
    std::vector<Stored> peoplemon;
};

} // namespace menu
} // namespace game

#endif
