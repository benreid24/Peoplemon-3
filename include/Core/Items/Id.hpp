#ifndef CORE_ITEMS_ID_HPP
#define CORE_ITEMS_ID_HPP

#include <cstdint>

/**
 * @addtogroup Items
 * @ingroup Core
 * @brief Collection of enums and classes for managing and using items
 *
 */

namespace core
{
/// Collection of enums and classes for managing and using items
namespace item
{
/**
 * @brief Represents an item in its simplist form
 *
 * @ingroup Items
 *
 */
enum struct Id : std::uint16_t {
    Unknown = 0,
    None    = Unknown,

    Potion      = 1,
    SuperPotion = 2,
    // TODO - item ids
    ID_COUNT
};

} // namespace item
} // namespace core

#endif
