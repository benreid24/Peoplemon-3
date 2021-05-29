#ifndef CORE_SCRIPTS_FUNCTIONS_HPP
#define CORE_SCRIPTS_FUNCTIONS_HPP

#include <BLIB/Scripts/SymbolTable.hpp>

/**
 * @addtogroup Scripts
 * @ingroup Core
 * @brief Collection of helpers and contexts for in-game scripts
 *
 */

namespace core
{
namespace system
{
class Systems;
}

/// Collection of helpers and contexts for in-game scripts
namespace script
{
/**
 * @brief Collection of static helpers to add builtin functions of various types to script symbol
 *        tables
 *
 * @ingroup Scripts
 *
 */
struct Functions {
    /**
     * @brief Adds the universal built-in functions to the given symbol table
     *
     * @param table The table to add the functions to
     * @param systems The primary systems objects
     */
    static void addDefaults(bl::script::SymbolTable& table, system::Systems& systems);
};

} // namespace script
} // namespace core

#endif
