#ifndef EDITOR_PAGES_SUBPAGES_TILESET_HPP
#define EDITOR_PAGES_SUBPAGES_TILESET_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/CatchRegion.hpp>
#include <Core/Maps/Tileset.hpp>
#include <Editor/Pages/Subpages/Catchables.hpp>
#include <Editor/Pages/Subpages/Collisions.hpp>

namespace editor
{
namespace component
{
class EditMap;
}

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
    using DeleteCb = std::function<void(core::map::Tile::IdType, bool)>;

    /**
     * @brief Creates the GUI elements
     *
     * @param deleteCb Called when a tile or animation is removed
     * @param map The map being edited
     *
     */
    Tileset(const DeleteCb& deleteCb, component::EditMap& map);

    /**
     * @brief Loads the given tileset and updates the GUI elements
     *
     * @param tileset The path to the tileset to load
     * @return True on success, false on error
     */
    bool loadTileset(const std::string& tileset);

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

    /**
     * @brief Returns the active catch type
     *
     */
    std::uint8_t getActiveCatch() const;

    /**
     * @brief Returns whether or not the tileset is in a dirty state
     *
     */
    bool unsavedChanges() const;

    /**
     * @brief Marks the tileset clean
     *
     */
    void markSaved();

    /**
     * @brief Refreshes the GUI
     *
     */
    void refresh();

private:
    const DeleteCb deleteCb;
    bl::resource::Resource<core::map::Tileset>::Ref tileset;

    bl::gui::Notebook::Ptr content;
    bl::gui::Box::Ptr tilesBox;
    bl::gui::Box::Ptr animsBox;

    Collisions collisions;
    Catchables catchables;

    Active tool;
    core::map::Tile::IdType activeTile;
    core::map::Tile::IdType activeAnim;
    bool dirty;

    void updateGui();
};

} // namespace page
} // namespace editor

#endif
