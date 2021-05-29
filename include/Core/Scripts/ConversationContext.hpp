#ifndef CORE_SCRIPTS_CONVERSATIONCONTEXT_HPP
#define CORE_SCRIPTS_CONVERSATIONCONTEXT_HPP

#include <BLIB/Entities/Entity.hpp>
#include <BLIB/Scripts.hpp>

namespace core
{
namespace system
{
class Systems;
}

namespace script
{
/**
 * @brief Special script context for conversation scripts. Adds the default Peoplemon built-ins and
 *        the conversation dependent built-ins
 *
 * @ingroup Scripts
 *
 */
class ConversationContext : public bl::script::Context {
public:
    /**
     * @brief Construct a new ConversationContext
     *
     * @param systems The primary systems object
     * @param entity The entity being conversed with
     */
    ConversationContext(system::Systems& systems, bl::entity::Entity entity);

protected:
    virtual void addCustomSymbols(bl::script::SymbolTable& table) const override;

private:
    system::Systems& systems;
    const bl::entity::Entity owner;
};

} // namespace script
} // namespace core

#endif
