#ifndef EDITOR_COMPONENTS_MOVEEDITORWINDOW_HPP
#define EDITOR_COMPONENTS_MOVEEDITORWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Editor/Components/TypeSelector.hpp>
#include <functional>
#include <Core/Files/MoveDB.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Editor window to edit moves in the move database
 *
 * @ingroup Components
 *
 */
class MoveEditorWindow {
public:
    /// Callback signature on move change
    using OnChange = std::function<void()>;

    /**
     * @brief Construct a new Move Editor Window
     *
     * @param moveDb The move database
     * @param onChange Callback for when a move is changed
     */
    MoveEditorWindow(core::file::MoveDB& moveDb, const OnChange& onChange);

    /**
     * @brief Opens the window and populates for the given move. Pass MoveId::Unknown for a new move
     *
     * @param parent The parent GUI object
     * @param move The move to populate for, or Unknown for a new move
     */
    void open(bl::gui::GUI* parent, core::pplmn::MoveId move);

private:
    core::file::MoveDB& moveDb;
    const OnChange onChange;
    bool doingNewMove;

    bl::gui::Window::Ptr window;
    bl::gui::TextEntry::Ptr idEntry;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr descEntry;
    TypeSelector::Ptr typeSelect;
    bl::gui::TextEntry::Ptr dmgEntry;
    bl::gui::TextEntry::Ptr accEntry;
    bl::gui::TextEntry::Ptr priorityEntry;
    bl::gui::TextEntry::Ptr ppEntry;
    bl::gui::CheckButton::Ptr specialCheck;
    bl::gui::CheckButton::Ptr contactCheck;
    bl::gui::ComboBox::Ptr effectSelect;
    bl::gui::TextEntry::Ptr effectChanceEntry;
    bl::gui::TextEntry::Ptr effectIntenseEntry;
    bl::gui::CheckButton::Ptr effectSelfCheck;

    core::pplmn::MoveId openId;
    bool dirty;
    bl::gui::Button::Ptr applyBut;
    
    void makeDirty();
    void onSave();
    void onCancel();
    void close();
};

} // namespace component
} // namespace editor

#endif
