#ifndef GAME_MENUS_STOREITEMROW_HPP
#define GAME_MENUS_STOREITEMROW_HPP

#include <BLIB/Graphics/Dummy2D.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Items/Id.hpp>

namespace game
{
namespace menu
{
/**
 * @brief Menu item for an item in the store. Does both sellable and for-sale items
 *
 * @ingroup Menus
 *
 */
class StoreItemRow : public bl::menu::Item {
public:
    /// Pointer to the menu item
    using Ptr = std::shared_ptr<StoreItemRow>;

    /// Height of a single item in pixels
    static constexpr float Height = 18.f;

    /**
     * @brief Creates a new menu item in sell mode
     *
     * @param qty The number of items available to be sold
     * @param item The item to sell
     * @param price The sale price
     * @return Ptr The new menu item
     */
    static Ptr create(int qty, core::item::Id item, int price);

    /**
     * @brief Creates a new menu item in buy mode
     *
     * @param item The item to buy
     * @param price The purchase price
     * @return Ptr The new menu item
     */
    static Ptr create(core::item::Id item, int price);

    /**
     * @brief Destroy the Store Item Row object
     *
     */
    virtual ~StoreItemRow() = default;

    /**
     * @brief Updates the text for how many items can be sold
     *
     * @param qty The number of items left in the bag
     */
    void updateQty(int qty);

    /**
     * @brief Returns the size that the row takes up
     *
     */
    virtual glm::vec2 getSize() const override;

    /**
     * @brief Returns the item that this row represents
     *
     */
    core::item::Id getItem() const;

protected:
    /**
     * @brief Called at least once when the item is added to a menu. Should create required graphics
     *        primitives and return the transform to use
     *
     * @param engine The game engine instance
     * @param parent The parent entity that should be used
     * @return The transform component to use
     */
    virtual bl::com::Transform2D& doCreate(bl::engine::Engine& engine,
                                           bl::ecs::Entity parent) override;

    /**
     * @brief Called when the item should be added to the overlay
     *
     * @param overlay The overlay to add to
     */
    virtual void doSceneAdd(bl::rc::Overlay* overlay) override;

    /**
     * @brief Called when the item should be removed from the overlay
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Returns the entity (or top level entity) of the item
     */
    virtual bl::ecs::Entity getEntity() const override;

private:
    const core::item::Id item;
    const int qty;
    const int price;
    bl::gfx::Dummy2D dummy;
    bl::gfx::Text qtyText;
    bl::gfx::Text nameText;
    bl::gfx::Text priceText;

    StoreItemRow(int qty, core::item::Id item, int price);
};

} // namespace menu
} // namespace game

#endif
