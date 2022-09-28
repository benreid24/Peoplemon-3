#ifndef GAME_MENUS_STOREITEMROW_HPP
#define GAME_MENUS_STOREITEMROW_HPP

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
    virtual sf::Vector2f getSize() const override;

    /**
     * @brief Returns the item that this row represents
     *
     */
    core::item::Id getItem() const;

protected:
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const override;

private:
    const core::item::Id item;
    sf::Text qtyText;
    sf::Text nameText;
    sf::Text priceText;

    StoreItemRow(int qty, core::item::Id item, int price);
};

} // namespace menu
} // namespace game

#endif
