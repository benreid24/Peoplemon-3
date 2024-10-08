#ifndef EDITOR_PAGES_SUBPAGES_TOWNS_HPP
#define EDITOR_PAGES_SUBPAGES_TOWNS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Town.hpp>
#include <Editor/Components/EditMap.hpp>
#include <Editor/Components/PlaylistEditorWindow.hpp>
#include <Editor/Components/WeatherSelect.hpp>

namespace editor
{
namespace page
{
class MapArea;

/**
 * @brief Subpage for creating, editing, deleting, and placing towns in maps
 *
 * @ingroup Pages
 */
class Towns {
public:
    /**
     * @brief Creates the towns subpage and GUI elements
     *
     * @param engine The game engine instance
     * @param map The edit map component
     */
    Towns(bl::engine::Engine& engine, MapArea& map);

    /**
     * @brief Returns the GUI element to pack
     */
    bl::gui::Element::Ptr getContent();

    /**
     * @brief Set the primary GUI object
     *
     * @param gui The primary GUI object
     */
    void setGUI(bl::gui::GUI* gui);

    /**
     * @brief Returns the currently selected town index
     */
    std::uint8_t selected() const;

    /**
     * @brief Returns the color to use for the given town
     */
    static sf::Color getColor(std::uint8_t index);

    /**
     * @brief Refreshes the list of towns in the page
     */
    void refresh();

private:
    bl::engine::Engine& engine;
    bl::gui::GUI* gui;
    MapArea& mapArea;
    component::EditMap& map;

    bl::gui::Box::Ptr content;
    bl::gui::ScrollArea::Ptr scrollRegion;
    bl::gui::RadioButton::Ptr noTownBut;
    std::uint8_t active;
    std::uint8_t editing;

    bl::gui::Window::Ptr window;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr descEntry;
    bl::gui::Label::Ptr playlistLabel;
    component::PlaylistEditorWindow playlistWindow;
    component::WeatherSelect::Ptr weatherSelect;
    bl::gui::ComboBox::Ptr spawnSelect;

    bl::rc::SceneRef scene;
    bl::rc::res::TextureRef flymapTxtr;
    bl::gfx::Sprite flyMap;
    bl::gfx::Circle townCircle;
    bl::gui::Canvas::Ptr mapPosCanvas;
    sf::Vector2i mapPos;

    void refreshSpawns(std::uint16_t spawn);
    void onPlaylistPick(const std::string& plst);
    void takeFocus();
    void closeWindow();

    void newTown();
    void editTown(std::uint8_t i);
    void onTownEdit();
    void removeTown(std::uint8_t i);
    bl::gui::Box::Ptr makeRow(std::uint8_t i, const std::string& town);

    void setMapPos(const bl::gui::Event& click);
    void refreshFlymapCanvas();
};

} // namespace page
} // namespace editor

#endif
