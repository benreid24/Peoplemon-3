#include <Core/Scripts/Functions.hpp>

#include <BLIB/Scripts.hpp>
#include <BLIB/Util/Waiter.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
using bl::script::Error;
using bl::script::SymbolTable;
using bl::script::Value;
using Function = bl::script::Function::CustomCB;
using ArgList  = const std::vector<Value>&;

namespace
{
typedef Value (*Builtin)(system::Systems& systems, ArgList args);

Value getPlayer(system::Systems& systems, ArgList);
Value giveItem(system::Systems& systems, ArgList args);
Value giveMoney(system::Systems& systems, ArgList args);
Value takeItem(system::Systems& systems, ArgList args);
Value takeMoney(system::Systems& systems, ArgList args);
Value givePeoplemon(system::Systems& systems, ArgList args);
Value takePeoplemon(system::Systems& systems, ArgList args);
Value whiteout(system::Systems& systems, ArgList args);
Value restorePeoplemon(system::Systems& systems, ArgList args);

Value displayMessage(system::Systems& systems, ArgList args);
Value promptPlayer(system::Systems& systems, ArgList args);
Value rollCredits(system::Systems& systems, ArgList args);

Value getNpc(system::Systems& systems, ArgList args);
Value spawnNpc(system::Systems& systems, ArgList args);
Value getTrainer(system::Systems& systems, ArgList args);
Value spawnTrainer(system::Systems& systems, ArgList args);

Value moveEntity(system::Systems& systems, ArgList args);
Value rotateEntity(system::Systems& systems, ArgList args);
Value removeEntity(system::Systems& systems, ArgList args);
Value entityToPosition(system::Systems& systems, ArgList args);
Value entityInteract(system::Systems& systems, ArgList args);
Value setEntityLock(system::Systems& systems, ArgList args);

Value getClock(system::Systems& systems, ArgList args);
Value waitUntilTime(system::Systems& systems, ArgList args);
Value runAtClockTime(system::Systems& systems, ArgList args);

Value addSaveEntry(system::Systems& systems, ArgList args);
Value getSaveEntry(system::Systems& systems, ArgList args);

Value makeBool(bool b) {
    Value v;
    v.makeBool(b);
    return v;
}

Value bind(system::Systems& systems, Builtin func) {
    return {
        Function([&systems, func](SymbolTable&, ArgList args) { return (*func)(systems, args); })};
}

} // namespace

void Functions::addDefaults(SymbolTable& table, system::Systems& systems) {
    table.set("getPlayer", bind(systems, &getPlayer));

    table.set("giveItem", bind(systems, &giveItem));
    table.set("giveMoney", bind(systems, &giveMoney));
    table.set("takeItem", bind(systems, &takeItem));
    table.set("takeMoney", bind(systems, &takeMoney));
    table.set("givePeoplemon", bind(systems, &givePeoplemon));
    table.set("takePeoplemon", bind(systems, &takePeoplemon));
    table.set("whiteout", bind(systems, &whiteout));
    table.set("restorePeoplemon", bind(systems, &restorePeoplemon));

    table.set("displayMessage", bind(systems, &displayMessage));
    table.set("promptPlayer", bind(systems, &promptPlayer));
    table.set("rollCredits", bind(systems, &rollCredits));

    table.set("getNpc", bind(systems, &getNpc));
    table.set("spawnNpc", bind(systems, &spawnNpc));
    table.set("getTrainer", bind(systems, &getTrainer));
    table.set("spawnTrainer", bind(systems, &spawnTrainer));

    table.set("moveEntity", bind(systems, &moveEntity));
    table.set("rotateEntity", bind(systems, &rotateEntity));
    table.set("removeEntity", bind(systems, &removeEntity));
    table.set("entityToPosition", bind(systems, &entityToPosition));
    table.set("entityInteract", bind(systems, &entityInteract));
    table.set("setEntityLock", bind(systems, &setEntityLock));

    table.set("getClock", bind(systems, &getClock));
    table.set("waitUntilTime", bind(systems, &waitUntilTime));
    table.set("runAtClockTime", bind(systems, &runAtClockTime));

    table.set("addSaveEntry", bind(systems, &addSaveEntry));
    table.set("getSaveEntry", bind(systems, &getSaveEntry));
}

namespace
{
Value makePosition(system::Systems& systems, bl::entity::Entity e) {
    Value value;
    const component::Position* pos =
        systems.engine().entities().getComponent<component::Position>(e);
    if (pos) {
        Value coord;
        coord.setProperty("x", {static_cast<float>(pos->positionTiles().x)});
        coord.setProperty("y", {static_cast<float>(pos->positionTiles().y)});
        value.setProperty("tiles", coord);
        coord.setProperty("x", {pos->positionPixels().x});
        coord.setProperty("y", {pos->positionPixels().y});
        value.setProperty("pixels", coord);
        value.setProperty("level", {static_cast<float>(pos->level)});
        value.setProperty("direction", {component::directionToString(pos->direction)});
    }
    return value;
}

template<Value::Type... Types>
void validateArgs(const std::string& func, ArgList args) {
    const Value::Type types[] = {Types...};
    const unsigned int nargs  = sizeof...(Types);

    if (args.size() != nargs)
        throw Error(func + "() takes " + std::to_string(nargs) + " arguments");
    for (unsigned int i = 0; i < nargs; ++i) {
        if (args[i].getType() != types[i]) {
            throw Error(func + "() argument " + std::to_string(i) + " must be a " +
                        Value::typeToString(types[i]) + " but " +
                        Value::typeToString(args[i].getType()) + " was passed");
        }
    }
}

Value getPlayer(system::Systems& systems, ArgList) {
    Value player(static_cast<float>(systems.player().player()));
    player.setProperty("name", {"player name"}); // TODO - store player name
    player.setProperty("gender", {"?"});         // TODO - store player gender
    player.setProperty("money", {100.f});        // TODO - track player money
    player.setProperty("position", makePosition(systems, systems.player().player()));

    std::vector<player::Bag::Item> items;
    systems.player().bag().getAll(items);
    std::vector<Value::Ptr> bag;
    bag.reserve(items.size());
    for (const player::Bag::Item& item : items) {
        bag.emplace_back(new Value(static_cast<float>(item.id)));
        bag.back()->setProperty("id", {static_cast<float>(item.id)});
        bag.back()->setProperty("name", {item::Item::getName(item.id)});
        bag.back()->setProperty("qty", {static_cast<float>(item.qty)});
    }
    player.setProperty("bag", {bag});

    return player;
}

Value giveItem(system::Systems& systems, ArgList args) {
    validateArgs<Value::TNumeric, Value::TNumeric, Value::TBool, Value::TBool>("giveItem", args);

    const unsigned int rawId = static_cast<unsigned int>(args[0].getAsNum());
    const item::Id item      = item::Item::cast(rawId);
    if (item != item::Id::Unknown) {
        const int qty = static_cast<int>(args[1].getAsNum());
        if (qty > 0) {
            systems.player().bag().addItem(item, qty);
            if (args[2].getAsBool()) {
                bl::util::Waiter waiter;
                system::HUD::Callback unlock = [](const std::string&) {};
                if (args[3].getAsBool())
                    unlock = [&waiter](const std::string&) { waiter.unblock(); };

                const std::string msg = qty > 1 ? ("Received " + std::to_string(qty) + " " +
                                                   item::Item::getName(item) + "s") :
                                                  ("Received a " + item::Item::getName(item));
                systems.hud().displayMessage(msg, unlock);
                if (args[3].getAsBool()) waiter.wait();
            }
        }
        else {
            BL_LOG_WARN << "qty must be a positive integer";
        }
    }
    else {
        BL_LOG_WARN << "Unknown item id: " << rawId;
    }
    return {};
}

Value giveMoney(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value takeItem(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value takeMoney(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value givePeoplemon(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value takePeoplemon(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value whiteout(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value restorePeoplemon(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value displayMessage(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value promptPlayer(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value rollCredits(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value getNpc(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value spawnNpc(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value getTrainer(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value spawnTrainer(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value moveEntity(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value rotateEntity(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value removeEntity(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value entityToPosition(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value entityInteract(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value setEntityLock(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value getClock(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value waitUntilTime(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value runAtClockTime(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value addSaveEntry(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value getSaveEntry(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

} // namespace

} // namespace script
} // namespace core
