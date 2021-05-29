#include <Core/Scripts/BaseFunctions.hpp>

#include <BLIB/Scripts.hpp>
#include <BLIB/Util/Waiter.hpp>
#include <Core/Components/NPC.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
using bl::script::Error;
using bl::script::Function;
using bl::script::SymbolTable;
using bl::script::Value;
using ArgList = const std::vector<Value>&;

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
Value getTrainer(system::Systems& systems, ArgList args);
Value spawnCharacter(system::Systems& systems, ArgList args);

Value moveEntity(system::Systems& systems, ArgList args);
Value rotateEntity(system::Systems& systems, ArgList args);
Value removeEntity(system::Systems& systems, ArgList args);
Value entityToPosition(system::Systems& systems, ArgList args);
Value entityInteract(system::Systems& systems, ArgList args);
Value setEntityLock(system::Systems& systems, ArgList args);
Value resetEntityLock(system::Systems& systems, ArgList args);

Value getClock(system::Systems& systems, ArgList args);
Value waitUntilTime(system::Systems& systems, ArgList args);
Value runAtClockTime(system::Systems& systems, ArgList args);

Value addSaveEntry(system::Systems& systems, ArgList args);
Value getSaveEntry(system::Systems& systems, ArgList args);

Value bind(system::Systems& systems, Builtin func) {
    return {
        Function([&systems, func](SymbolTable&, ArgList args) { return (*func)(systems, args); })};
}

} // namespace

void BaseFunctions::addDefaults(SymbolTable& table, system::Systems& systems) {
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
    table.set("getTrainer", bind(systems, &getTrainer));
    table.set("spawnCharacter", bind(systems, &spawnCharacter));

    table.set("moveEntity", bind(systems, &moveEntity));
    table.set("rotateEntity", bind(systems, &rotateEntity));
    table.set("removeEntity", bind(systems, &removeEntity));
    table.set("entityToPosition", bind(systems, &entityToPosition));
    table.set("entityInteract", bind(systems, &entityInteract));
    table.set("setEntityLock", bind(systems, &setEntityLock));
    table.set("resetEntityLock", bind(systems, &resetEntityLock));

    table.set("getClock", bind(systems, &getClock));
    table.set("waitUntilTime", bind(systems, &waitUntilTime));
    table.set("runAtClockTime", bind(systems, &runAtClockTime));

    table.set("addSaveEntry", bind(systems, &addSaveEntry));
    table.set("getSaveEntry", bind(systems, &getSaveEntry));
}

namespace
{
Value makeBool(bool b) {
    Value v;
    v.makeBool(b);
    return v;
}

Value makePosition(const component::Position& pos) {
    Value value;
    Value coord;
    coord.setProperty("x", {static_cast<float>(pos.positionTiles().x)});
    coord.setProperty("y", {static_cast<float>(pos.positionTiles().y)});
    value.setProperty("tiles", coord);
    coord.setProperty("x", {pos.positionPixels().x});
    coord.setProperty("y", {pos.positionPixels().y});
    value.setProperty("pixels", coord);
    value.setProperty("level", {static_cast<float>(pos.level)});
    value.setProperty("direction", {component::directionToString(pos.direction)});
    return value;
}

Value makePosition(system::Systems& systems, bl::entity::Entity e) {
    const component::Position* pos =
        systems.engine().entities().getComponent<component::Position>(e);
    if (pos) { return makePosition(*pos); }
    return {};
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
    Function::validateArgs<Value::TNumeric, Value::TNumeric, Value::TBool, Value::TBool>("giveItem",
                                                                                         args);

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
    Function::validateArgs<Value::TNumeric, Value::TBool, Value::TBool>("giveMoney", args);

    const int money = static_cast<unsigned int>(args[0].getAsNum());
    if (money > 0) {
        // TODO - track and give player money
        if (args[1].getAsBool()) {
            bl::util::Waiter waiter;
            system::HUD::Callback unlock = [](const std::string&) {};
            if (args[2].getAsBool()) unlock = [&waiter](const std::string&) { waiter.unblock(); };

            const std::string msg = "Received " + std::to_string(money) + " monies";
            systems.hud().displayMessage(msg, unlock);
            if (args[2].getAsBool()) waiter.wait();
        }
    }
    else {
        BL_LOG_WARN << "qty must be a positive integer";
    }
    return {};
}

Value takeItem(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric, Value::TNumeric, Value::TBool>("takeItem", args);

    const unsigned int rawId = static_cast<unsigned int>(args[0].getAsNum());
    const item::Id item      = item::Item::cast(rawId);
    if (item != item::Id::Unknown) {
        const int qty = static_cast<int>(args[1].getAsNum());
        if (qty > 0) {
            if (args[2].getAsBool()) {
                bl::util::Waiter waiter;
                std::string choice;
                system::HUD::Callback unlock = unlock = [&waiter, &choice](const std::string& c) {
                    choice = c;
                    waiter.unblock();
                };

                const std::string msg =
                    qty > 1 ?
                        ("Give " + std::to_string(qty) + " " + item::Item::getName(item) + "s?") :
                        ("Give the " + item::Item::getName(item) + "?");
                systems.hud().promptUser(msg, {"Yes", "No"}, unlock);
                waiter.wait();
                if (choice == "No") return makeBool(false);
            }
            return makeBool(systems.player().bag().removeItem(item, qty));
        }
        else {
            BL_LOG_WARN << "qty must be a positive integer";
        }
    }
    else {
        BL_LOG_WARN << "Unknown item id: " << rawId;
    }
    return makeBool(false);
}

Value takeMoney(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric, Value::TBool>("takeMoney", args);

    const int qty = static_cast<int>(args[0].getAsNum());
    if (qty > 0) {
        if (args[1].getAsBool()) {
            bl::util::Waiter waiter;
            std::string choice;
            system::HUD::Callback unlock = unlock = [&waiter, &choice](const std::string& c) {
                choice = c;
                waiter.unblock();
            };

            const std::string msg = "Give " + std::to_string(qty) + " monies?";
            systems.hud().promptUser(msg, {"Yes", "No"}, unlock);
            waiter.wait();
            if (choice == "No") return makeBool(false);
        }
        return makeBool(false); // TODO - track and take player money
    }
    else {
        BL_LOG_WARN << "qty must be a positive integer";
    }
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
    Function::validateArgs<Value::TString, Value::TBool>("displayMessage", args);

    bl::util::Waiter waiter;
    system::HUD::Callback unlock = [](const std::string&) {};
    if (args[1].getAsBool()) unlock = [&waiter](const std::string&) { waiter.unblock(); };

    systems.hud().displayMessage(args[0].getAsString(), unlock);
    if (args[1].getAsBool()) waiter.wait();
    return {};
}

Value promptPlayer(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TString, Value::TArray>("promptPlayer", args);

    const auto rawChoices = args[1].getAsArray();
    std::vector<std::string> choices;
    choices.reserve(rawChoices.size());
    for (const Value::Ptr& val : rawChoices) {
        if (val->getType() != Value::TString) {
            throw Error("All choices in promptPlayer() must be String type");
        }
        choices.emplace_back(val->getAsString());
    }

    bl::util::Waiter waiter;
    std::string choice;
    const auto cb = [&waiter, &choice](const std::string& c) {
        choice = c;
        waiter.unblock();
    };
    systems.hud().promptUser(args[0].getAsString(), choices, cb);
    waiter.wait();

    return {choice};
}

Value rollCredits(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value getNpc(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TString>("getNpc", args);

    const std::string name = args[0].getAsString();
    const auto npcMap      = systems.engine()
                            .entities()
                            .getEntitiesWithComponents<component::NPC, component::Position>()
                            ->results();
    for (const auto& pair : npcMap) {
        if (pair.second.get<component::NPC>()->name() == name) {
            Value npc(static_cast<float>(pair.first));
            npc.setProperty("name", {name});
            npc.setProperty("talkedTo", makeBool(false)); // TODO - track who talked to
            npc.setProperty("defeated", makeBool(false));
            npc.setProperty("position", makePosition(*pair.second.get<component::Position>()));
            return npc;
        }
    }

    return makeBool(false);
}

Value spawnCharacter(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TString,
                           Value::TNumeric,
                           Value::TNumeric,
                           Value::TNumeric,
                           Value::TString>("spawnCharacter", args);

    const map::CharacterSpawn spawn(
        component::Position(
            static_cast<std::uint8_t>(args[1].getAsNum()),
            {static_cast<int>(args[2].getAsNum()), static_cast<int>(args[3].getAsNum())},
            component::directionFromString(args[4].getAsString())),
        args[0].getAsString());
    return makeBool(systems.entity().spawnCharacter(spawn));
}

Value getTrainer(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TString>("getTrainer", args);

    const std::string name = args[0].getAsString();
    const auto trainerMap =
        systems.engine()
            .entities()
            .getEntitiesWithComponents<component::Trainer, component::Position>()
            ->results();
    for (const auto& pair : trainerMap) {
        if (pair.second.get<component::Trainer>()->name() == name) {
            Value trainer(static_cast<float>(pair.first));
            trainer.setProperty("name", {name});
            trainer.setProperty("talkedTo", makeBool(false)); // TODO - track who talked to
            trainer.setProperty("defeated", makeBool(false)); // TODO - track who defeated
            trainer.setProperty("position", makePosition(*pair.second.get<component::Position>()));
            return trainer;
        }
    }

    return makeBool(false);
}

Value moveEntity(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric, Value::TString, Value::TBool>("moveEntity", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].getAsNum());
    const component::Direction dir  = component::directionFromString(args[1].getAsString());
    const bool block                = args[2].getAsBool();
    const bool result               = systems.movement().moveEntity(entity, dir, false);

    if (block && result) {
        const component::Movable* move =
            systems.engine().entities().getComponent<component::Movable>(entity);
        if (move) {
            float t = 0.f;
            const float mtime =
                Properties::PixelsPerTile() / Properties::CharacterMoveSpeed() * 1.5f;
            while (move->moving()) {
                sf::sleep(sf::milliseconds(10));
                t += 0.01f;
                if (t >= mtime) {
                    BL_LOG_WARN << "Blocking on moveEntity for entity " << entity
                                << " taking too long (" << t << "s), continuing";
                    break;
                }
            }
        }
    }

    return makeBool(result);
}

Value rotateEntity(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric, Value::TString, Value::TBool>("rotateEntity", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].getAsNum());
    const component::Direction dir  = component::directionFromString(args[1].getAsString());
    component::Position* pos =
        systems.engine().entities().getComponent<component::Position>(entity);
    if (pos && pos->direction != dir) pos->direction = dir;

    return {};
}

Value removeEntity(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric>("removeEntity", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].getAsNum());
    if (entity != systems.player().player()) {
        const bool result = systems.engine().entities().entityExists(entity);
        systems.engine().entities().destroyEntity(entity);
        return makeBool(result);
    }
    return makeBool(false);
}

Value entityToPosition(system::Systems& systems, ArgList args) {
    // TODO
    return makeBool(false);
}

Value entityInteract(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric>("entityInteract", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].getAsNum());
    return makeBool(systems.interaction().interact(entity));
}

Value setEntityLock(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric, Value::TBool>("setEntityLock", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].getAsNum());
    const bool lock                 = args[1].getAsBool();
    systems.controllable().setEntityLocked(entity, lock, true);
    return {};
}

Value resetEntityLock(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric>("resetEntityLock", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].getAsNum());
    systems.controllable().resetEntityLock(entity);
    return {};
}

Value getClock(system::Systems& systems, ArgList args) {
    const system::Clock::Time now = systems.clock().now();
    Value clock(static_cast<float>(now.hour * 60 + now.minute));
    clock.setProperty("minute", {static_cast<float>(now.minute)});
    clock.setProperty("hour", {static_cast<float>(now.hour)});
    clock.setProperty("day", {static_cast<float>(now.day)});
    return clock;
}

class ClockTrigger : public bl::event::Listener<event::TimeChange> {
public:
    using Callback = std::function<void()>;
    ClockTrigger(Callback cb, const system::Clock::Time& time)
    : cb(cb)
    , time(time) {}

    virtual void observe(const event::TimeChange& event) override {
        if (event.newTime.hour == time.hour && event.newTime.minute == time.minute) cb();
    }

private:
    const system::Clock::Time time;
    const Callback cb;
};

system::Clock::Time makeTime(const Value& v) {
    system::Clock::Time t(12, 0, 0);
    auto c = v.getProperty("hour");
    if (c && c->getType() == Value::TNumeric) { // TODO - move deref into Value to allow references
                                                // natively everywhere?
        t.hour = static_cast<unsigned int>(c->getAsNum());
    }
    c = v.getProperty("minute");
    if (c && c->getType() == Value::TNumeric) {
        t.minute = static_cast<unsigned int>(c->getAsNum());
    }
    return t;
}

Value waitUntilTime(system::Systems& systems, ArgList args) {
    Function::validateArgs<Value::TNumeric, Value::TBool>("waitUntilTime", args);

    const system::Clock::Time now  = systems.clock().now();
    const system::Clock::Time then = makeTime(args[0]);
    const unsigned int nts         = now.hour * 60 + now.minute;
    const unsigned int ets         = then.hour * 60 + then.minute;
    if (nts < ets || (nts != ets && args[1].getAsBool())) {
        bl::util::Waiter waiter;
        const auto unlock = [&waiter]() { waiter.unblock(); };
        ClockTrigger trigger(unlock, then);
        bl::event::ClassGuard<event::TimeChange> guard(&trigger);
        guard.subscribe(systems.engine().eventBus());
        waiter.wait();
    }

    return {};
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