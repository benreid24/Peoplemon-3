#ifndef EDITOR_COMPONENTS_ITEMSELECTOR_HPP
#define EDITOR_COMPONENTS_ITEMSELECTOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Items/Item.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Wrapper over ComboBox that allows selection of any item in the item database
 *
 * @ingroup UIComponents
 *
 */
class ItemSelector : public bl::gui::ComboBox {
public:
    /// Pointer to this component
    using Ptr = std::shared_ptr<ItemSelector>;

    /// Called when the item changes
    using ChangeCb = std::function<void(core::item::Id)>;

    /**
     * @brief Creates a new ItemSelector
     *
     * @param cb The function to call when the item is changed
     *
     */
    static Ptr create(const ChangeCb& cb = {});

    /**
     * @brief Returns the current selected item
     *
     * @return core::item::Id
     */
    core::item::Id currentItem() const;

    /**
     * @brief Set the current item
     *
     */
    void setItem(core::item::Id item);

    /**
     * @brief Refreshes the items from the item database
     *
     */
    void refresh();

private:
    static std::vector<core::item::Id> idLookup;

    ItemSelector(const ChangeCb& cb = {});
};

} // namespace component
} // namespace editor

#endif
