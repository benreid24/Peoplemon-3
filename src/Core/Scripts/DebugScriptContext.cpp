#include <Core/Scripts/DebugScriptContext.hpp>

#include <Core/Scripts/BaseFunctions.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
DebugScriptContext::DebugScriptContext(system::Systems& s)
: systems(s) {}

void DebugScriptContext::addCustomSymbols(bl::script::SymbolTable& table) const {
    BaseFunctions::addDefaults(table, systems);
}

} // namespace script
} // namespace core
