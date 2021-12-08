#ifndef CORE_FILES_ITEMDB_HPP
#define CORE_FILES_ITEMDB_HPP

#include <BLIB/Serialization/Binary.hpp>
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
struct ItemDB : private bl::util::NonCopyable {
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

    std::unordered_map<item::Id, std::string> names;
    std::unordered_map<item::Id, std::string> descriptions;
    std::unordered_map<item::Id, int> values;

    friend class ItemDBLoader;
};

} // namespace file
} // namespace core

namespace bl
{
namespace serial
{
namespace binary
{
template<>
struct SerializableObject<core::file::ItemDB> : public SerializableObjectBase {
    using Id = core::item::Id;
    using DB = core::file::ItemDB;

    SerializableField<1, std::unordered_map<Id, std::string>, offsetof(DB, names)> names;
    SerializableField<2, std::unordered_map<Id, std::string>, offsetof(DB, descriptions)>
        descriptions;
    SerializableField<3, std::unordered_map<Id, int>, offsetof(DB, values)> values;

    SerializableObject()
    : names(*this)
    , descriptions(*this)
    , values(*this) {}
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
