#ifndef CORE_SCRIPTS_MAPEVENTCONTEXT_HPP
#define CORE_SCRIPTS_MAPEVENTCONTEXT_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Scripts.hpp>
#include <Core/Components/Position.hpp>
#include <Core/Maps/Event.hpp>

namespace core
{
namespace system
{
class Systems;
}

namespace script
{
/**
 * @brief Special script context for map events. Adds default built-ins and the map event built-ins
 *
 * @ingroup Scripts
 *
 */
class MapEventContext : public bl::script::Context {
public:
    /**
     * @brief Construct a new MapEventContext
     *
     * @param systems The primary systems object
     * @param entity The entity that triggered the event
     * @param event The event that the entity triggered
     * @param tile The position of the entity when the event fired
     */
    MapEventContext(system::Systems& systems, bl::ecs::Entity entity, const map::Event& event,
                    const component::Position& tile);

protected:
    virtual void addCustomSymbols(bl::script::SymbolTable& table) const override;

private:
    system::Systems& systems;
    const bl::ecs::Entity entity;
    const map::Event event;
    const component::Position tile;
};

} // namespace script
} // namespace core

#endif
