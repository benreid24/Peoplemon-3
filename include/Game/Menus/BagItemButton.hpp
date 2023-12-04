#ifndef GAME_MENUS_BAGITEMBUTTON_HPP
#define GAME_MENUS_BAGITEMBUTTON_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Player/Bag.hpp>

/**
 * @addtogroup Menus
 * @brief Collection of menu components used in various game states
 * @ingroup Game
 *
 */

namespace game
{
namespace menu
{
/**
 * @brief Represents a button for a quantity of items in the bag
 *
 * @ingroup Menus
 */
class BagItemButton : public bl::menu::Item {
public:
    /// Pointer to the menu item
    using Ptr = std::shared_ptr<BagItemButton>;

    /**
     * @brief Creates a new item button
     *
     * @param item The item to populate with
     * @return Ptr The created item
     */
    static Ptr create(const core::player::Bag::Item& item);

    /**
     * @brief Destroy the Bag Item Button object
     */
    virtual ~BagItemButton() = default;

    /**
     * @brief Updates the text labels from the given item
     */
    void update(const core::player::Bag::Item& item);

    /**
     * @brief Returns the size the button takes up
     */
    virtual glm::vec2 getSize() const override;

    /**
     * @brief Returns the item this button is representing
     */
    const core::player::Bag::Item& getItem() const;

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
    core::player::Bag::Item item;
    bl::rc::res::TextureRef txtr;
    bl::gfx::Sprite background;
    bl::gfx::Text label;
    bl::gfx::Text qty;

    BagItemButton(const core::player::Bag::Item& item);
    void onSelect();
    void onDeselect();
};

} // namespace menu
} // namespace game

#endif
