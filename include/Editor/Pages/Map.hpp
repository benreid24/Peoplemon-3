#ifndef EDITOR_PAGES_MAP_HPP
#define EDITOR_PAGES_MAP_HPP

#include <Editor/Pages/Page.hpp>
#include <Editor/Pages/Subpages/Layers.hpp>
#include <Editor/Pages/Subpages/Tileset.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for map editing. Tiles, levels, events, NPC's, weather, lighting, everything
 *
 * @ingroup Pages
 *
 */
class Map : public Page {
public:
    /**
     * @brief Construct a new Map page
     *
     * @param systems The primary systems object
     */
    Map(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Map() = default;

    /**
     * @brief Updates the game world and systems
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

private:
    bl::gui::Box::Ptr controlPane;
    bl::gui::Notebook::Ptr controlBook;
    bl::gui::Notebook::Ptr objectBook;

    // TODO - map render area element

    bl::gui::Box::Ptr mapCtrlBox;
    bl::gui::Button::Ptr newMapBut;
    bl::gui::Button::Ptr loadMapBut;
    bl::gui::Button::Ptr saveMapBut;

    bl::gui::Notebook::Ptr editBook;
    bl::gui::Box::Ptr tileBox;
    bl::gui::ComboBox::Ptr layerSelect;
    bl::gui::ComboBox::Ptr levelSelect;
    bl::gui::RadioButton::Ptr tileSetBut;
    bl::gui::RadioButton::Ptr tileClearBut;
    bl::gui::RadioButton::Ptr tileSelectBut;
    bl::gui::Button::Ptr tileDeselectBut;
    bl::gui::Button::Ptr tileLayerUpBut;
    bl::gui::Button::Ptr tileLayerDownBut;
    bl::gui::Button::Ptr tileLevelUpBut;
    bl::gui::Button::Ptr tileLevelDownBut;

    bl::gui::Box::Ptr infoBox;
    bl::gui::Label::Ptr nameLabel;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::Button::Ptr resizeBut;
    bl::gui::ComboBox::Ptr weatherEntry;
    bl::gui::Label::Ptr playlistLabel;
    bl::gui::Button::Ptr pickPlaylistBut;

    Tileset tileset;

    Layers levelPage;
    Layers layerPage;
};

} // namespace page
} // namespace editor

#endif
