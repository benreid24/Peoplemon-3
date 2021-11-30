#ifndef CORE_FILES_ITEMDB_HPP
#define CORE_FILES_ITEMDB_HPP

#include <BLIB/Files/Binary.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <Core/Items/Category.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Items/Type.hpp>
#include <string>
#include <unordered_map>

namespace core
{
namespace file
{
class ItemDBLoader;

/**
 * @brief Loads and stores metadata surrounding items in the game
 *
 * @ingroup Files
 *
 */
struct ItemDB
: private bl::file::binary::SerializableObject
, private bl::util::NonCopyable {
    /**
     * @brief Creates an empty database
     *
     */
    ItemDB();

    /**
     * @brief Loads the item metadata from the data file
     *
     * @return True on success false on error
     */
    bool load();

    /**
     * @brief Writes the item metadata to the data file
     *
     * @return True on success false on error
     */
    bool save() const;

#define SF bl::file::binary::SerializableField
    SF<1, std::unordered_map<item::Id, std::string>> names;
    SF<2, std::unordered_map<item::Id, std::string>> descriptions;
    SF<3, std::unordered_map<item::Id, int>> values;
#undef SF

    friend class ItemDBLoader;
};

} // namespace file
} // namespace core

#endif
