#ifndef CORE_SCRIPTS_CONVERSATIONCONTEXT_HPP
#define CORE_SCRIPTS_CONVERSATIONCONTEXT_HPP

#include <BLIB/ECS/Entity.hpp>
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
     * @brief Special callback type for reporting conversation state
     *
     * @param entity The entity to check status for
     * @return bool True if conversation completed, false if ongoing
     *
     */
    using StatusCb = std::function<bool(bl::ecs::Entity)>;

    /**
     * @brief Construct a new ConversationContext
     *
     * @param systems The primary systems object
     * @param entity The entity being conversed with
     * @param cb Callback that can be used to query the conversation state
     */
    ConversationContext(system::Systems& systems, bl::ecs::Entity entity, const StatusCb& cb);

protected:
    virtual void addCustomSymbols(bl::script::SymbolTable& table) const override;

private:
    system::Systems& systems;
    const StatusCb cb;
    const bl::ecs::Entity owner;
};

} // namespace script
} // namespace core

#endif
