#ifndef CORE_MAPS_ITEM_HPP
#define CORE_MAPS_ITEM_HPP

#include <BLIB/Serialization.hpp>

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
struct Item {
    std::uint16_t id;
    std::uint16_t mapId;
    sf::Vector2i position;
    std::uint8_t level;
    bool visible;

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
     * @param level The level the item is on
     * @param visible True to render the item, false to hide
     */
    Item(std::uint16_t id, std::uint16_t mapId, const sf::Vector2i& pos, std::uint8_t level,
         bool visible);
};

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<core::map::Item> : public SerializableObjectBase {
    using I = core::map::Item;

    SerializableField<1, I, std::uint16_t> id;
    SerializableField<2, I, std::uint16_t> mapId;
    SerializableField<3, I, sf::Vector2i> position;
    SerializableField<4, I, std::uint8_t> level;
    SerializableField<5, I, bool> visible;

    SerializableObject()
    : id("id", *this, &I::id, SerializableFieldBase::Required{})
    , mapId("mapid", *this, &I::mapId, SerializableFieldBase::Required{})
    , position("position", *this, &I::position, SerializableFieldBase::Required{})
    , level("level", *this, &I::level, SerializableFieldBase::Required{})
    , visible("visible", *this, &I::visible, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
