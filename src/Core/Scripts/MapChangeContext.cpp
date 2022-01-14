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

typedef void (*Builtin)(const std::string&, const std::string&, std::uint16_t, Value&);

void mapName(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn,
             Value& result);
void previousMap(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn,
                 Value& result);
void spawnId(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn,
             Value& result);

Value bind(const std::string& prevMap, const std::string& newMap, std::uint16_t spawn,
           Builtin func) {
    return Value(Function(std::bind(func, prevMap, newMap, spawn, std::placeholders::_3)));
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
void mapName(const std::string&, const std::string& newMap, std::uint16_t, Value& result) {
    result = newMap;
}

void previousMap(const std::string& prevMap, const std::string&, std::uint16_t, Value& result) {
    result = prevMap;
}

void spawnId(const std::string&, const std::string&, std::uint16_t spawn, Value& result) {
    result = static_cast<float>(spawn);
}

} // namespace

} // namespace script
} // namespace core
