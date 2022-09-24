#ifndef EDITOR_PAGES_ITEMS_HPP
#define EDITOR_PAGES_ITEMS_HPP

#include <Editor/Pages/Page.hpp>

#include <Core/Files/ItemDB.hpp>
#include <Editor/Components/ItemEditorWindow.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for editing item names, descriptions, and values
 *
 * @ingroup Pages
 *
 */
class Items : public Page {
public:
    /**
     * @brief Construct a new Items page
     *
     * @param systems The primary systems object
     */
    Items(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Items() = default;

    /**
     * @brief Does nothing
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

private:
    core::file::ItemDB itemDb;
    component::ItemEditorWindow itemWindow;
    bl::gui::Button::Ptr saveBut;
    bl::gui::ScrollArea::Ptr rowArea;

    void onChange();
    void makeDirty();
    void save();
    void newItem();
    void editItem(core::item::Id move);
    void deleteItem(core::item::Id move);
    void sync();
    void reset();
};

} // namespace page
} // namespace editor

#endif
