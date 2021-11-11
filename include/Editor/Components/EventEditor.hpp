#ifndef EDITOR_COMPONENTS_EVENTEDITOR_HPP
#define EDITOR_COMPONENTS_EVENTEDITOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Map.hpp>
#include <Editor/Components/ScriptSelector.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Map event editor dialog
 *
 * @ingroup UIComponents
 *
 */
class EventEditor {
public:
    /// Callback called when an event is created or modified
    using OnEdit = std::function<void(const core::map::Event*, const core::map::Event&)>;

    /**
     * @brief Construct a new Event Editor dialog
     *
     * @param onEdit Callback to call when an event is created or modified
     */
    EventEditor(const OnEdit& onEdit);

    /**
     * @brief Opens the dialog window and optionally populates with event info
     *
     * @param parent The parent GUI object
     * @param source Optional event to populate from
     * @param pos The position to populate if not editing an existing event
     */
    void open(const bl::gui::GUI::Ptr& parent, const core::map::Event* source,
              const sf::Vector2i& pos);

private:
    const OnEdit onEdit;
    const core::map::Event* orig;
    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    bl::gui::Label::Ptr scriptLabel;
    bl::gui::TextEntry::Ptr xInput;
    bl::gui::TextEntry::Ptr yInput;
    bl::gui::TextEntry::Ptr wInput;
    bl::gui::TextEntry::Ptr hInput;
    bl::gui::ComboBox::Ptr triggerSelect;

    ScriptSelector scriptSelector;
    void onScriptChosen(const std::string& s);

    bool valid() const;
    core::map::Event makeEvent() const;
};

} // namespace component
} // namespace editor

#endif
