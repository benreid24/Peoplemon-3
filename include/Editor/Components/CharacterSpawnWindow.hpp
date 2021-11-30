#ifndef EDITOR_COMPONENTS_CHARACTERSPAWNWINDOW_HPP
#define EDITOR_COMPONENTS_CHARACTERSPAWNWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/CharacterSpawn.hpp>
#include <Editor/Components/NPCEditorWindow.hpp>
#include <Editor/Components/TrainerEditorWindow.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Editing window for character spawns. Manages NPCs and trainers
 *
 * @ingroup UIComponents
 *
 */
class CharacterSpawnWindow {
public:
    /// Callback for when spawns are created or edited
    using OnEdit =
        std::function<void(const core::map::CharacterSpawn*, const core::map::CharacterSpawn&)>;

    /**
     * @brief Creates a new CharacterSpawnWindow
     *
     * @param onEdit Callback for when spawns are edited
     */
    CharacterSpawnWindow(const OnEdit& onEdit);

    /**
     * @brief Opens the spawn editing window
     *
     * @param parent The parent GUI object
     * @param level The level the spawn is on
     * @param pos The position the spawn is at
     * @param orig The spawn currently in that spot. Nullptr if none
     */
    void open(const bl::gui::GUI::Ptr& parent, unsigned int level, const sf::Vector2i& pos,
              const core::map::CharacterSpawn* orig);

private:
    const OnEdit onEdit;
    const core::map::CharacterSpawn* orig;
    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    bl::gui::Label::Ptr fileLabel;
    bl::gui::TextEntry::Ptr levelInput;
    bl::gui::TextEntry::Ptr xInput;
    bl::gui::TextEntry::Ptr yInput;
    bl::gui::ComboBox::Ptr dirEntry;

    component::NpcEditorWindow npcEditor;
    component::TrainerEditorWindow trainerEditor;
    void onFilechoose(const std::string& file);
    void closeAll();
};

} // namespace component
} // namespace editor

#endif
