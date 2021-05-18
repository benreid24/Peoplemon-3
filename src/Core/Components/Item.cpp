#include <Core/Components/Item.hpp>

namespace core
{
namespace component
{
Item::Item(item::Id item)
: item(item) {}

item::Id Item::id() const { return item; }

} // namespace component
} // namespace core
