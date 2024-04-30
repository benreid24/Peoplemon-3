#ifndef GAME_MENUS_STORAGEGRID_HPP
#define GAME_MENUS_STORAGEGRID_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Peoplemon/StoredPeoplemon.hpp>
#include <functional>
#include <list>
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
    static constexpr glm::vec2 BoxPosition = {293.f, 158.f};
    static constexpr glm::vec2 BoxSize     = {784.f - BoxPosition.x, 581.f - BoxPosition.y};

    /**
     * @brief Initializes the grid to be empty
     *
     * @param engine The game engine instance
     */
    StorageGrid(bl::engine::Engine& engine);

    /**
     * @brief Sets the parent of the grid rectangle to the background
     *
     * @param background The background entity
     */
    void activate(bl::ecs::Entity background);

    /**
     * @brief Removes the content from the scene and releases entities
     */
    void deactivate();

    /**
     * @brief Updates the set of peoplemon to render
     *
     * @param box The set of stored peoplemon to render
     */
    void update(const std::vector<core::pplmn::StoredPeoplemon>& box);

    /**
     * @brief Applies the given offset to the grid position. Used for sliding in and out
     *
     * @param offset The offset in overlay units to apply to the x coordinate
     */
    void notifyOffset(float offset);

private:
    bl::engine::Engine& engine;
    bl::gfx::Dummy2D background;
    std::list<bl::gfx::Sprite> peoplemon;
};

} // namespace menu
} // namespace game

#endif
