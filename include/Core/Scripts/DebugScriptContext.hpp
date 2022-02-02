#ifndef CORE_SCRIPTS_DEBUGSCRIPTCONTEXT_HPP
#define CORE_SCRIPTS_DEBUGSCRIPTCONTEXT_HPP

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
 * @brief Script context for debug scripts ran from the terminal
 *
 * @ingroup Scripts
 *
 */
class DebugScriptContext : public bl::script::Context {
public:
    /**
     * @brief Construct a new Debug Script Context
     *
     * @param systems The main game systems
     */
    DebugScriptContext(system::Systems& systems);

protected:
    virtual void addCustomSymbols(bl::script::SymbolTable& table) const override;

private:
    system::Systems& systems;
};

} // namespace script
} // namespace core

#endif
