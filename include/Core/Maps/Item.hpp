#ifndef CORE_MAPS_ITEM_HPP
#define CORE_MAPS_ITEM_HPP

#include <BLIB/Files/Binary.hpp>

namespace core
{
namespace map
{
/**
 * @brief Basic struct representing a pickup-able item in Map
 *
 * @ingroup Maps
 *
 */
struct Item : public bl::file::binary::SerializableObject {
    bl::file::binary::SerializableField<1, std::uint16_t> id;
    bl::file::binary::SerializableField<2, std::uint16_t> mapId;
    bl::file::binary::SerializableField<3, sf::Vector2i> position;

    /**
     * @brief Makes an empty item
     *
     */
    Item();

    /**
     * @brief Makes an item from the given parameters
     *
     * @param id The id of the item that gets added to the bag
     * @param mapId A map unique id to avoid spawning picked up items
     * @param pos The position of the item, in tiles
     */
    Item(std::uint16_t id, std::uint16_t mapId, const sf::Vector2i& pos);

    /**
     * @brief Copy constructs the item from the given item
     *
     * @param copy The item to copy
     */
    Item(const Item& copy);

    /**
     * @brief Copies from the given item
     *
     * @param copy The item to copy
     * @return Item& A reference to this item
     */
    Item& operator=(const Item& copy);
};

} // namespace map
} // namespace core

#endif
