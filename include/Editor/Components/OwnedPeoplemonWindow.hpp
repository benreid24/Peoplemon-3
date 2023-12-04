#ifndef EDITOR_COMPONENTS_OWNEDPEOPLEMONWINDOW_HPP
#define EDITOR_COMPONENTS_OWNEDPEOPLEMONWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Editor/Components/ItemSelector.hpp>
#include <Editor/Components/MoveSelector.hpp>
#include <Editor/Components/StatBox.hpp>
#include <Editor/Components/PeoplemonSelector.hpp>

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
    void show(bl::gui::GUI* parent, const core::pplmn::OwnedPeoplemon& value = {});

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
    bl::gui::GUI* parent;
    bl::gui::Window::Ptr window;

    PeoplemonSelector::Ptr idSelect;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr levelEntry;
    ItemSelector::Ptr itemSelector;

    MoveSelector::Ptr moveSelector;
    bl::gui::SelectBox::Ptr moveBox;
    std::vector<core::pplmn::MoveId> moves;
    void syncMoves();

    StatBox evBox;
    StatBox ivBox;

    bool validate() const;
};

} // namespace component
} // namespace editor

#endif
