#ifndef CORE_SCRIPTS_MAPCHANGECONTEXT_HPP
#define CORE_SCRIPTS_MAPCHANGECONTEXT_HPP

#include <BLIB/Scripts.hpp>
#include <cstdint>

namespace core
{
namespace system
{
class Systems;
}

namespace script
{
/**
 * @brief Special context for map enter and exit scripts
 *
 * @ingroup Scripts
 *
 */
class MapChangeContext : public bl::script::Context {
public:
    /**
     * @brief Construct a new Map ChangeContext
     *
     * @param systems The primary systems object
     * @param prevMap The name of the previous map
     * @param newMap The name of the new map
     * @param spawn The spawn id of the player spawn
     */
    MapChangeContext(system::Systems& systems, const std::string& prevMap,
                     const std::string& newMap, std::uint16_t spawn);

protected:
    virtual void addCustomSymbols(bl::script::SymbolTable& table) const override;

private:
    system::Systems& systems;
    const std::string prevMap;
    const std::string newMap;
    const std::uint16_t spawn;
};

} // namespace script
} // namespace core

#endif
