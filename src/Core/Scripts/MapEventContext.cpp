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

typedef Value (*Builtin)(bl::entity::Entity, const component::Position&, const map::Event&);

Value triggeringEntity(bl::entity::Entity entity, const component::Position& pos,
                       const map::Event& event);

Value triggerPosition(bl::entity::Entity entity, const component::Position& pos,
                      const map::Event& event);

Value eventType(bl::entity::Entity entity, const component::Position& pos, const map::Event& event);

Value eventPosition(bl::entity::Entity entity, const component::Position& pos,
                    const map::Event& event);

Value eventSize(bl::entity::Entity entity, const component::Position& pos, const map::Event& event);

Value bind(bl::entity::Entity entity, const component::Position& pos, const map::Event& event,
           Builtin func) {
    return {Function([entity, pos, &event, func](SymbolTable&, const std::vector<Value>&) {
        return (*func)(entity, pos, event);
    })};
}

} // namespace

MapEventContext::MapEventContext(system::Systems& systems, bl::entity::Entity entity,
                                 const map::Event& event, const component::Position& tile)
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
Value triggeringEntity(bl::entity::Entity entity, const component::Position&, const map::Event&) {
    return {static_cast<float>(entity)};
}

Value triggerPosition(bl::entity::Entity, const component::Position& pos, const map::Event&) {
    return BaseFunctions::makePosition(pos);
}

Value eventType(bl::entity::Entity, const component::Position&, const map::Event& event) {
    switch (event.trigger.getValue()) {
    case map::Event::Trigger::OnEnter:
        return {"OnEnter"};
    case map::Event::Trigger::OnExit:
        return {"OnExit"};
    case map::Event::Trigger::onEnterOrExit:
        return {"onEnterOrExit"};
    case map::Event::Trigger::WhileIn:
        return {"WhileIn"};
    case map::Event::Trigger::OnInteract:
        return {"OnInteract"};
    default:
        return {"Unknown"};
    }
}

Value eventPosition(bl::entity::Entity, const component::Position&, const map::Event& event) {
    Value coord;
    coord.setProperty("x", {static_cast<float>(event.position.getValue().x)});
    coord.setProperty("y", {static_cast<float>(event.position.getValue().y)});
    return coord;
}

Value eventSize(bl::entity::Entity, const component::Position&, const map::Event& event) {
    Value size;
    size.setProperty("x", {static_cast<float>(event.areaSize.getValue().x)});
    size.setProperty("y", {static_cast<float>(event.areaSize.getValue().y)});
    return size;
}

} // namespace

} // namespace script
} // namespace core
