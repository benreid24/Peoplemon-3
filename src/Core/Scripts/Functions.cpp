#include <Core/Scripts/Functions.hpp>

#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
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
Value getPlayer(system::Systems& systems, ArgList) {
    // TODO
    return makeBool(false);
}

Value giveItem(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
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
