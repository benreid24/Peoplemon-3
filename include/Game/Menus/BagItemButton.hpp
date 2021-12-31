#ifndef GAME_MENUS_BAGITEMBUTTON_HPP
#define GAME_MENUS_BAGITEMBUTTON_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Player/Bag.hpp>

namespace game
{
namespace menu
{
/**
 * @brief Represents a button for a quantity of items in the bag
 *
 * @ingroup Menus
 *
 */
class BagItemButton : public bl::menu::Item {
public:
    /// Pointer to the menu item
    using Ptr = std::shared_ptr<BagItemButton>;

    /**
     * @brief Creates a new item button
     *
     * @param item The item to popuate with
     * @return Ptr The created item
     */
    static Ptr create(const core::player::Bag::Item& item);

    /**
     * @brief Destroy the Bag Item Button object
     *
     */
    virtual ~BagItemButton() = default;

    /**
     * @brief Updates the text labels from the given item
     *
     */
    void update(const core::player::Bag::Item& item);

    /**
     * @brief Returns the size the button takes up
     *
     */
    virtual sf::Vector2f getSize() const override;

    /**
     * @brief Returns the item this button is representing
     *
     */
    const core::player::Bag::Item& getItem() const;

protected:
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const override;

private:
    core::player::Bag::Item item;
    bl::resource::Resource<sf::Texture>::Ref txtr;
    sf::Sprite background;
    sf::Text label;
    sf::Text qty;

    BagItemButton(const core::player::Bag::Item& item);
    void onSelect();
    void onDeselect();
};

} // namespace menu
} // namespace game

#endif
