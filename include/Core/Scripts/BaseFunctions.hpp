#ifndef CORE_SCRIPTS_BASEFUNCTIONS_HPP
#define CORE_SCRIPTS_BASEFUNCTIONS_HPP

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
 * @brief Adds the built-in default functions to all Peoplemon scripts
 *
 * @ingroup Scripts
 *
 */
struct BaseFunctions {
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
