#include <Core/Scripts/MapChangeContext.hpp>

#include <Core/Scripts/BaseFunctions.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
using bl::script::Function;
using bl::script::SymbolTable;
using bl::script::Value;

namespace
{
// TODO - functions
}

MapChangeContext::MapChangeContext(system::Systems& systems, const std::string& prevMap,
                                   const std::string& newMap, std::uint16_t spawn)
: systems(systems)
, prevMap(prevMap)
, newMap(newMap)
, spawn(spawn) {}

void MapChangeContext::addCustomSymbols(SymbolTable& table) const {
    BaseFunctions::addDefaults(table, systems);

    // TODO - add
}

namespace
{
// TODO - functions
}

} // namespace script
} // namespace core
