#ifndef EDITOR_PAGES_MAP_HPP
#define EDITOR_PAGES_MAP_HPP

#include <Editor/Components/CharacterSpawnWindow.hpp>
#include <Editor/Components/EventEditor.hpp>
#include <Editor/Components/ItemSelector.hpp>
#include <Editor/Components/LightSlider.hpp>
#include <Editor/Components/NewMapDialog.hpp>
#include <Editor/Components/PlaylistEditorWindow.hpp>
#include <Editor/Components/ScriptSelector.hpp>
#include <Editor/Components/WeatherSelect.hpp>
#include <Editor/Pages/Page.hpp>
#include <Editor/Pages/Subpages/Layers.hpp>
#include <Editor/Pages/Subpages/Levels.hpp>
#include <Editor/Pages/Subpages/MapArea.hpp>
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
    bl::gui::ComboBox::Ptr layerSelect;
    bl::gui::ComboBox::Ptr levelSelect;

    bl::gui::TextEntry::Ptr nameEntry;
    component::WeatherSelect::Ptr weatherEntry;
    bl::gui::Label::Ptr playlistLabel;
    bl::gui::Button::Ptr saveMapBut;

    bl::gui::CheckButton::Ptr sunlightBut;
    component::LightSlider::Ptr minLightSlider;
    component::LightSlider::Ptr maxLightSlider;
    bl::gui::Button::Ptr lightingSetBut;

    bl::gui::Notebook::Ptr objectBook;
    bl::gui::ComboBox::Ptr spawnDirEntry;
    bl::gui::TextEntry::Ptr lightRadiusEntry;
    bl::gui::RadioButton::Ptr lightCreateOrEdit;
    bl::gui::RadioButton::Ptr lightRemove;
    component::ItemSelector::Ptr itemSelector;

    bl::gui::RadioButton::Ptr createEventRadio;
    bl::gui::RadioButton::Ptr editEventRadio;
    bl::gui::RadioButton::Ptr deleteEventRadio;

    bl::gui::RadioButton::Ptr spawnCreate;
    bl::gui::RadioButton::Ptr spawnRotate;
    bl::gui::RadioButton::Ptr spawnDelete;

    bl::gui::RadioButton::Ptr npcSpawn;
    bl::gui::RadioButton::Ptr npcEdit;
    bl::gui::RadioButton::Ptr npcDelete;

    bl::gui::RadioButton::Ptr itemSpawn;
    bl::gui::RadioButton::Ptr itemRead;
    bl::gui::RadioButton::Ptr itemDelete;
    bl::gui::CheckButton::Ptr itemHidden;

    bl::gui::Label::Ptr onEnterLabel;
    bl::gui::Label::Ptr onExitLabel;

    component::WeatherSelect::Ptr tempWeatherEntry;
    bl::gui::ComboBox::Ptr timeSetEntry;

    MapArea mapArea;
    Tileset tileset;
    Levels levelPage;
    Layers layerPage;

    enum struct Tool { Metadata, MapEdit, Events, Spawns, NPCs, Items, Lights } activeTool;
    enum struct Subtool { Set, Fill, Select, Clear, Add, Edit, Remove, None } activeSubtool;

    sf::IntRect selection;
    enum SelectionState { NoSelection, Selecting, SelectionMade } selectionState;

    bl::gui::FilePicker mapPicker;
    bool makingNewMap;
    void doLoadMap(const std::string& file);

    component::NewMapDialog newMapWindow;
    void makeNewMap(const std::string& file, const std::string& name, const std::string& tileset,
                    unsigned int w, unsigned int h);

    component::PlaylistEditorWindow playlistEditor;
    void onChoosePlaylist(const std::string& file);

    void syncLighting();
    void onLightingChange();
    void onLightingSave();
    void onLightingReset();
    void setLightingDefault();

    component::ScriptSelector scriptSelector;
    bool choosingOnloadScript;
    void onChooseScript(const std::string& script);

    component::EventEditor eventEditor;
    void onEventEdit(const core::map::Event* orig, const core::map::Event& event);

    void onMapClick(const sf::Vector2f& pixels, const sf::Vector2i& tiles);
    void onLevelChange(unsigned int level);

    component::CharacterSpawnWindow characterEditor;
    void onCharacterEdit(const core::map::CharacterSpawn* orig,
                         const core::map::CharacterSpawn& spawn);

    bool checkUnsaved();
    void syncGui();
};

} // namespace page
} // namespace editor

#endif
