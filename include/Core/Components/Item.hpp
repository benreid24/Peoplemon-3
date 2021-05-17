#ifndef CORE_COMPONENTS_ITEM_HPP
#define CORE_COMPONENTS_ITEM_HPP

#include <BLIB/Entities.hpp>
#include <Core/Items/Id.hpp>

namespace core
{
namespace component
{
/**
 * @brief Add this component to an entity to make it give an item when interacted with. On interact
 *        the owning entity is removed
 *
 * @ingroup Components
 *
 */
class Item {
public:
    /// Required for BLIB ECS
    static constexpr bl::entity::Component::IdType ComponentId = 11;

    /**
     * @brief Construct a new Item component
     *
     * @param id The item to give on interaction
     */
    Item(item::Id id);

    /**
     * @brief Returns the item id of this item entity
     *
     */
    item::Id id() const;

private:
    const item::Id item;
};

} // namespace component
} // namespace core

#endif
