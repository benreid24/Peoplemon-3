#ifndef EDITOR_COMPONENTS_ITEMEDITORWINDOW_HPP
#define EDITOR_COMPONENTS_ITEMEDITORWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/ItemDB.hpp>
#include <Core/Items/Item.hpp>
#include <functional>

namespace editor
{
namespace component
{
/**
 * @brief Window for editing an individual item in the item database
 *
 * @ingroup Components
 *
 */
class ItemEditorWindow {
public:
    /// Callback signature for when an item is modified
    using OnChange = std::function<void()>;

    /**
     * @brief Construct a new Item Editor Window
     *
     * @param db The item database
     * @param onChange Callback for when an item is changed
     */
    ItemEditorWindow(core::file::ItemDB& db, const OnChange& onChange);

    /**
     * @brief Open the editor window
     *
     * @param parent The parent GUI object
     * @param item The item to populate for, or Unknown for a new item
     */
    void open(const bl::gui::GUI::Ptr& parent, core::item::Id item);

private:
    core::file::ItemDB& itemDb;
    const OnChange onChange;
    bool doingNewItem;

    bl::gui::Window::Ptr window;
    bl::gui::TextEntry::Ptr idEntry;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr descEntry;
    bl::gui::TextEntry::Ptr valueEntry;

    core::item::Id openId;
    bool dirty;
    bl::gui::Button::Ptr applyBut;

    void makeDirty();
    void onCancel();
    void onSave();
    void close();
};

} // namespace component
} // namespace editor

#endif
