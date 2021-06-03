#ifndef EDITOR_PAGES_SUBPAGES_TILESET_HPP
#define EDITOR_PAGES_SUBPAGES_TILESET_HPP

#include <BLIB/Interfaces/GUI.hpp>

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
    /**
     * @brief Creates the GUI elements
     *
     */
    Tileset();

    /**
     * @brief Loads the given tileset and initializes the GUI for it
     *
     * @param file The tilset to load
     * @return True on success, false on error
     */
    bool loadTileset(const std::string& file); // TODO - take actual tileset?

    // TODO - events/accessors for current state or deleted tiles

private:
    bl::gui::Notebook content;
    bl::gui::Box::Ptr tilePage;
    bl::gui::Box::Ptr animPage;

    bl::gui::Box::Ptr tileButBox;
    bl::gui::Button::Ptr addTileBut;
    bl::gui::Button::Ptr importSpritesheetBut;
    bl::gui::Button::Ptr delTileBut;

    bl::gui::Box::Ptr animButBox;
    bl::gui::Button::Ptr addAnimBut;
    bl::gui::Button::Ptr delAnimBut;

    // TODO - radio button image buttons for tiles/anims
};

} // namespace page
} // namespace editor

#endif
