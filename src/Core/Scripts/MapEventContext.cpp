#include <Core/Scripts/MapEventContext.hpp>

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

typedef void (*Builtin)(bl::ecs::Entity, const bl::tmap::Position&, const map::Event&, Value&);

void triggeringEntity(bl::ecs::Entity entity, const bl::tmap::Position& pos,
                      const map::Event& event, Value&);

void triggerPosition(bl::ecs::Entity entity, const bl::tmap::Position& pos,
                     const map::Event& event, Value&);

void eventType(bl::ecs::Entity entity, const bl::tmap::Position& pos, const map::Event& event,
               Value&);

void eventPosition(bl::ecs::Entity entity, const bl::tmap::Position& pos, const map::Event& event,
                   Value&);

void eventSize(bl::ecs::Entity entity, const bl::tmap::Position& pos, const map::Event& event,
               Value&);

Value bind(bl::ecs::Entity entity, const bl::tmap::Position& pos, const map::Event& event,
           Builtin func) {
    return Value(Function(std::bind(func, entity, pos, event, std::placeholders::_3)));
}

} // namespace

MapEventContext::MapEventContext(system::Systems& systems, bl::ecs::Entity entity,
                                 const map::Event& event, const bl::tmap::Position& tile)
: systems(systems)
, entity(entity)
, event(event)
, tile(tile) {}

void MapEventContext::addCustomSymbols(SymbolTable& table) const {
    BaseFunctions::addDefaults(table, systems);

    table.set("triggeringEntity", bind(entity, tile, event, &triggeringEntity));
    table.set("triggerPosition", bind(entity, tile, event, &triggerPosition));
    table.set("eventType", bind(entity, tile, event, &eventType));
    table.set("eventPosition", bind(entity, tile, event, &eventPosition));
    table.set("eventSize", bind(entity, tile, event, &eventSize));
}

namespace
{
void triggeringEntity(bl::ecs::Entity entity, const bl::tmap::Position&, const map::Event&,
                      Value& result) {
    result = static_cast<float>(entity);
}

void triggerPosition(bl::ecs::Entity, const bl::tmap::Position& pos, const map::Event&,
                     Value& result) {
    result = BaseFunctions::makePosition(pos);
}

void eventType(bl::ecs::Entity, const bl::tmap::Position&, const map::Event& event,
               Value& result) {
    switch (event.trigger) {
    case map::Event::Trigger::OnEnter:
        result = "OnEnter";
        break;
    case map::Event::Trigger::OnExit:
        result = "OnExit";
        break;
    case map::Event::Trigger::onEnterOrExit:
        result = "onEnterOrExit";
        break;
    case map::Event::Trigger::WhileIn:
        result = "WhileIn";
        break;
    case map::Event::Trigger::OnInteract:
        result = "OnInteract";
        break;
    default:
        result = "Unknown";
    }
}

void eventPosition(bl::ecs::Entity, const bl::tmap::Position&, const map::Event& event,
                   Value& coord) {
    coord.setProperty("x", {static_cast<float>(event.position.x)});
    coord.setProperty("y", {static_cast<float>(event.position.y)});
}

void eventSize(bl::ecs::Entity, const bl::tmap::Position&, const map::Event& event, Value& size) {
    size.setProperty("x", {static_cast<float>(event.areaSize.x)});
    size.setProperty("y", {static_cast<float>(event.areaSize.y)});
}

} // namespace

} // namespace script
} // namespace core
