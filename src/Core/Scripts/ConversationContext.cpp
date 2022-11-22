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

typedef void (*Builtin)(SymbolTable&, bl::ecs::Entity, const ConversationContext::StatusCb&,
                        Value&);

void talkingEntity(SymbolTable& table, bl::ecs::Entity entity,
                   const ConversationContext::StatusCb& cb, Value& result);
void conversationOver(SymbolTable& table, bl::ecs::Entity entity,
                      const ConversationContext::StatusCb& cb, Value& result);
void waitConversationOver(SymbolTable& table, bl::ecs::Entity entity,
                          const ConversationContext::StatusCb& cb, Value& result);

Value bind(bl::ecs::Entity entity, const ConversationContext::StatusCb& cb, Builtin func) {
    return Value(
        Function(std::bind(func, std::placeholders::_1, entity, cb, std::placeholders::_3)));
}

} // namespace

ConversationContext::ConversationContext(system::Systems& sys, bl::ecs::Entity owner,
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
void talkingEntity(SymbolTable&, bl::ecs::Entity entity, const ConversationContext::StatusCb&,
                   Value& res) {
    res = static_cast<float>(entity);
}

void conversationOver(SymbolTable&, bl::ecs::Entity entity, const ConversationContext::StatusCb& cb,
                      Value& res) {
    res = cb(entity);
}

void waitConversationOver(SymbolTable& table, bl::ecs::Entity entity,
                          const ConversationContext::StatusCb& cb, Value&) {
    while (!cb(entity)) { table.waitFor(100); }
}

} // namespace

} // namespace script
} // namespace core
