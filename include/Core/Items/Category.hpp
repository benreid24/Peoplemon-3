#ifndef CORE_ITEMS_CATEGORY_HPP
#define CORE_ITEMS_CATEGORY_HPP

#include <cstdint>

namespace core
{
namespace item
{
/**
 * @brief Represents a category that an item can belong to. Used for bag sorting
 *
 * @ingroup Items
 *
 */
enum struct Category : std::uint8_t {
    /// Unknown item category
    Unknown,

    /// Regular items. Covers ids [1, 100]
    Regular,

    /// Key items. Covers ids [101, 200]
    Key,

    /// TM's. Covers ids [201, 499]
    TM,

    /// Peopleballs
    Peopleball
};

} // namespace item
} // namespace core

#endif
