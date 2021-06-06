#ifndef EDITOR_PAGES_SUBPAGES_TILESET_HPP
#define EDITOR_PAGES_SUBPAGES_TILESET_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Editor/Pages/Subpages/Catchables.hpp>
#include <Editor/Pages/Subpages/Collisions.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Subpage for tileset editing and use. Used by the map editor page
 *
 * @ingroup Pages
 *
 */
class Tileset {
public:
    enum Active { Tiles, Animations, CollisionTiles, CatchTiles };

    /**
     * @brief Creates the GUI elements
     *
     */
    Tileset();

    /**
     * @brief Returns the gui element to pack
     *
     */
    bl::gui::Element::Ptr getContent();

    /**
     * @brief Returns what is currently selected as the active edit type
     *
     */
    Active getActiveTool() const;

    /**
     * @brief Returns the id of the active tile
     *
     */
    core::map::Tile::IdType getActiveTile() const;

    /**
     * @brief Returns the id of the active animation
     *
     */
    core::map::Tile::IdType getActiveAnim() const;

    /**
     * @brief Returns the active collision type
     *
     */
    core::map::Collision getActiveCollision() const;

private:
    bl::gui::Notebook::Ptr content;
    bl::gui::Box::Ptr tilesBox;
    bl::gui::Box::Ptr animsBox;

    // TODO - radio button image buttons for tiles/anims

    Collisions collisions;
    Catchables catchables;
};

} // namespace page
} // namespace editor

#endif
