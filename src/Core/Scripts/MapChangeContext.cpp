#include <Core/Scripts/MapChangeContext.hpp>

#include <Core/Scripts/BaseFunctions.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
namespace
{
using bl::script::Function;
using bl::script::SymbolTable;
using bl::script::Value;

typedef Value (*Builtin)(const std::string&, const std::string&, std::uint16_t);

Value mapName(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn);
Value previousMap(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn);
Value spawnId(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn);

Value bind(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn,
           Builtin func) {
    return {Function([prevMap, newMap, spawn, func](SymbolTable&, const std::vector<Value>&) {
        return (*func)(prevMap, newMap, spawn);
    })};
}

} // namespace

MapChangeContext::MapChangeContext(system::Systems& systems, const std::string& prevMap,
                                   const std::string& newMap, std::uint16_t spawn)
: systems(systems)
, prevMap(prevMap)
, newMap(newMap)
, spawn(spawn) {}

void MapChangeContext::addCustomSymbols(SymbolTable& table) const {
    BaseFunctions::addDefaults(table, systems);

    table.set("mapName", bind(prevMap, newMap, spawn, &mapName));
    table.set("previousMap", bind(prevMap, newMap, spawn, &previousMap));
    table.set("spawnId", bind(prevMap, newMap, spawn, &spawnId));
}

namespace
{
Value mapName(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn) {
    return {newMap};
}

Value previousMap(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn) {
    return {prevMap};
}

Value spawnId(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn) {
    return {static_cast<float>(spawn)};
}

} // namespace

} // namespace script
} // namespace core
