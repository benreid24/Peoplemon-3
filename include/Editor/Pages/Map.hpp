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
    // TODO - map render area element

    bl::gui::ComboBox::Ptr layerSelect;
    bl::gui::ComboBox::Ptr levelSelect;

    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::ComboBox::Ptr weatherEntry;
    bl::gui::Label::Ptr playlistLabel;

    bl::gui::ComboBox::Ptr spawnDirEntry;
    bl::gui::TextEntry::Ptr lightRadiusEntry;
    bl::gui::ComboBox::Ptr itemSpawnEntry;
    std::vector<core::item::Id> itemIdLookup;

    bl::gui::Label::Ptr onEnterLabel;
    bl::gui::Label::Ptr onExitLabel;

    Tileset tileset;
    Layers levelPage;
    Layers layerPage;
};

} // namespace page
} // namespace editor

#endif
