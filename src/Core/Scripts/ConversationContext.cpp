#include <Core/Scripts/ConversationContext.hpp>

#include <Core/Scripts/BaseFunctions.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
ConversationContext::ConversationContext(system::Systems& sys, bl::entity::Entity owner)
: systems(sys)
, owner(owner) {}

void ConversationContext::addCustomSymbols(bl::script::SymbolTable& table) const {
    BaseFunctions::addDefaults(table, systems);
    // TODO - add conversation symbols here
}

} // namespace script
} // namespace core
