#ifndef EDITOR_COMPONENTS_OWNEDPEOPLEMONWINDOW_HPP
#define EDITOR_COMPONENTS_OWNEDPEOPLEMONWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Editor/Components/ItemSelector.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Window for editing owned Peoplemon
 *
 * @ingroup EditorComponents
 *
 */
class OwnedPeoplemonWindow {
public:
    /// Callback for when the window is closed
    using NotifyCB = std::function<void()>;

    /**
     * @brief Construct a new Owned Peoplemon Window
     *
     * @param onFinish Called when the Peoplemon is finished
     * @param onCancel Called when the window is closed without using the peoplemon
     */
    OwnedPeoplemonWindow(const NotifyCB& onFinish, const NotifyCB& onCancel);

    /**
     * @brief Shows the window
     *
     * @param parent The parent GUI object
     * @param value The value to fill with
     */
    void show(const bl::gui::GUI::Ptr& parent, const core::pplmn::OwnedPeoplemon& value = {});

    /**
     * @brief Hides the window
     *
     */
    void hide();

    /**
     * @brief Returns the value of the Peoplemon entered
     *
     */
    core::pplmn::OwnedPeoplemon getValue() const;

private:
    const NotifyCB onFinish;
    const NotifyCB onCancel;
    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    std::unordered_map<int, core::pplmn::Id> idMap;

    bl::gui::ComboBox::Ptr idSelect;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr levelEntry;
    ItemSelector::Ptr itemSelector;

    // TODO - move selectors

    bl::gui::TextEntry::Ptr evHpEntry;
    bl::gui::TextEntry::Ptr evAtkEntry;
    bl::gui::TextEntry::Ptr evDefEntry;
    bl::gui::TextEntry::Ptr evSpAtkEntry;
    bl::gui::TextEntry::Ptr evSpDefEntry;
    bl::gui::TextEntry::Ptr evSpdEntry;

    bl::gui::TextEntry::Ptr ivHpEntry;
    bl::gui::TextEntry::Ptr ivAtkEntry;
    bl::gui::TextEntry::Ptr ivDefEntry;
    bl::gui::TextEntry::Ptr ivSpAtkEntry;
    bl::gui::TextEntry::Ptr ivSpDefEntry;
    bl::gui::TextEntry::Ptr ivSpdEntry;

    bool validate() const;
    void syncMoves(core::pplmn::Id ppl);
};

} // namespace component
} // namespace editor

#endif
