#include <Core/Scripts/ConversationContext.hpp>

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

typedef Value (*Builtin)(SymbolTable&, bl::entity::Entity, const ConversationContext::StatusCb&);

Value talkingEntity(SymbolTable& table, bl::entity::Entity entity,
                    const ConversationContext::StatusCb& cb);
Value conversationOver(SymbolTable& table, bl::entity::Entity entity,
                       const ConversationContext::StatusCb& cb);
Value waitConversationOver(SymbolTable& table, bl::entity::Entity entity,
                           const ConversationContext::StatusCb& cb);

Value bind(bl::entity::Entity entity, const ConversationContext::StatusCb& cb, Builtin func) {
    return {Function([entity, cb, func](SymbolTable& table, const std::vector<Value>&) {
        return (*func)(table, entity, cb);
    })};
}

} // namespace

ConversationContext::ConversationContext(system::Systems& sys, bl::entity::Entity owner,
                                         const StatusCb& cb)
: systems(sys)
, cb(cb)
, owner(owner) {}

void ConversationContext::addCustomSymbols(SymbolTable& table) const {
    BaseFunctions::addDefaults(table, systems);
    table.set("talkingEntity", bind(owner, cb, &talkingEntity));
    table.set("conversationOver", bind(owner, cb, &conversationOver));
    table.set("waitConversationOver", bind(owner, cb, &waitConversationOver));
}

namespace
{
Value talkingEntity(SymbolTable&, bl::entity::Entity entity, const ConversationContext::StatusCb&) {
    return {static_cast<float>(entity)};
}

Value conversationOver(SymbolTable&, bl::entity::Entity entity,
                       const ConversationContext::StatusCb& cb) {
    Value v;
    v.makeBool(cb(entity));
    return v;
}

Value waitConversationOver(SymbolTable& table, bl::entity::Entity entity,
                           const ConversationContext::StatusCb& cb) {
    while (!cb(entity)) { table.waitFor(100); }
    return {};
}

} // namespace

} // namespace script
} // namespace core
