#ifndef CORE_ITEMS_USERESULT_HPP
#define CORE_ITEMS_USERESULT_HPP

#include <string>

namespace core
{
namespace item
{
/**
 * @brief Basic struct to store the result of using an item
 *
 * @ingroup Items
 *
 */
struct UseResult {
    /// True if the item was used, false on error
    const bool used;

    /// The message to display on use or error
    const std::string message;

    /**
     * @brief Construct a new UseResult object
     *
     * @param used True if the item was used
     * @param message The message to show
     */
    UseResult(bool used, const std::string& message);
};

} // namespace item
} // namespace core

#endif
