#include <Core/Scripts/MapEventContext.hpp>

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

MapEventContext::MapEventContext(system::Systems& systems, bl::entity::Entity entity,
                                 const map::Event& event, const component::Position& tile)
: systems(systems)
, entity(entity)
, event(event)
, tile(tile) {}

void MapEventContext::addCustomSymbols(SymbolTable& table) const {
    BaseFunctions::addDefaults(table, systems);

    // TODO - add map functions
}

namespace
{
// TODO - functions
}

} // namespace script
} // namespace core
