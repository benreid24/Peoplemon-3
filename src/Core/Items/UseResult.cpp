#include <Core/Items/UseResult.hpp>

namespace core
{
namespace item
{
UseResult::UseResult(bool u, const std::string& s)
: used(u)
, message(s) {}

} // namespace item
} // namespace core
