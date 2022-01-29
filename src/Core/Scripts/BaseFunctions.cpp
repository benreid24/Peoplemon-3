#include <Core/Scripts/BaseFunctions.hpp>

#include <BLIB/Scripts.hpp>
#include <BLIB/Util/Waiter.hpp>
#include <Core/Components/NPC.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Events/Maps.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace script
{
using bl::script::Error;
using bl::script::Function;
using bl::script::PrimitiveValue;
using bl::script::SymbolTable;
using bl::script::Value;

namespace
{
constexpr PrimitiveValue::Type EntryTypes =
    static_cast<PrimitiveValue::Type>(0xFF ^ (PrimitiveValue::TFunction | PrimitiveValue::TVoid));

typedef void (*Builtin)(system::Systems& systems, SymbolTable& table,
                        const std::vector<Value>& args, Value& result);

void getPlayer(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
               Value& result);
void giveItem(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result);
void giveMoney(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
               Value& result);
void takeItem(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result);
void takeMoney(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
               Value& result);
void givePeoplemon(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value& result);
void takePeoplemon(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value& result);
void whiteout(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result);
void restorePeoplemon(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                      Value& result);

void displayMessage(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                    Value& result);
void promptPlayer(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                  Value& result);
void rollCredits(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                 Value& result);

void getNpc(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
            Value& result);
void getTrainer(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                Value& result);
void loadCharacter(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value& result);
void spawnCharacter(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                    Value& result);

void moveEntity(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                Value& result);
void rotateEntity(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                  Value& result);
void removeEntity(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                  Value& result);
void entityToPosition(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                      Value& result);
void entityInteract(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                    Value& result);
void setEntityLock(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value& result);
void resetEntityLock(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                     Value& result);

void makeTime(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result);
void getClock(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result);
void waitUntilTime(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value& result);
void runAtClockTime(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                    Value& result);

void addSaveEntry(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                  Value& result);
void getSaveEntry(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                  Value& result);
void checkConvFlag(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value& result);
void setConvFlag(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                 Value& result);

void loadMap(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
             Value& result);
void setAmbientLight(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                     Value& result);
void createLight(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                 Value& result);
void updateLight(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                 Value& result);
void removeLight(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                 Value& result);

void clearWeather(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                  Value& result);
void makeRain(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result);
void makeSnow(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result);
void makeSunny(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
               Value& result);
void makeSandstorm(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value& result);
void makeFog(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
             Value& result);
void makeRandomRain(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                    Value& result);
void makeRandomSnow(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                    Value& result);
void makeRandomDesert(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                      Value& result);
void makeRandomWeather(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                       Value& result);
void getCurrentWeather(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                       Value& result);

Value bind(system::Systems& systems, Builtin func) {
    return {Function([&systems, func](SymbolTable& table,
                                      const std::vector<Value>& args,
                                      Value& result) { (*func)(systems, table, args, result); })};
}

} // namespace

void BaseFunctions::addDefaults(SymbolTable& table, system::Systems& systems) {
#define BUILTIN(function) table.set(#function, bind(systems, &function))

    BUILTIN(getPlayer);

    BUILTIN(giveItem);
    BUILTIN(giveMoney);
    BUILTIN(takeItem);
    BUILTIN(takeMoney);
    BUILTIN(givePeoplemon);
    BUILTIN(takePeoplemon);
    BUILTIN(whiteout);
    BUILTIN(restorePeoplemon);

    BUILTIN(displayMessage);
    BUILTIN(promptPlayer);
    BUILTIN(rollCredits);

    BUILTIN(getNpc);
    BUILTIN(getTrainer);
    BUILTIN(loadCharacter);
    BUILTIN(spawnCharacter);

    BUILTIN(moveEntity);
    BUILTIN(rotateEntity);
    BUILTIN(removeEntity);
    BUILTIN(entityToPosition);
    BUILTIN(entityInteract);
    BUILTIN(setEntityLock);
    BUILTIN(resetEntityLock);

    BUILTIN(makeTime);
    BUILTIN(getClock);
    BUILTIN(waitUntilTime);
    BUILTIN(runAtClockTime);

    BUILTIN(addSaveEntry);
    BUILTIN(getSaveEntry);
    BUILTIN(checkConvFlag);
    BUILTIN(setConvFlag);

    BUILTIN(loadMap);
    BUILTIN(setAmbientLight);
    BUILTIN(createLight);
    BUILTIN(updateLight);
    BUILTIN(removeLight);

    BUILTIN(clearWeather);
    BUILTIN(makeRain);
    BUILTIN(makeSnow);
    BUILTIN(makeSunny);
    BUILTIN(makeSandstorm);
    BUILTIN(makeFog);
    BUILTIN(makeRandomRain);
    BUILTIN(makeRandomSnow);
    BUILTIN(makeRandomDesert);
    BUILTIN(makeRandomWeather);
    BUILTIN(getCurrentWeather);

#undef BUILTIN
}

Value BaseFunctions::makePosition(const component::Position& pos) {
    Value value;
    Value coord;
    coord.setProperty("x", {pos.positionTiles().x});
    coord.setProperty("y", {pos.positionTiles().y});
    value.setProperty("tiles", coord);
    coord.setProperty("x", {pos.positionPixels().x});
    coord.setProperty("y", {pos.positionPixels().y});
    value.setProperty("pixels", coord);
    value.setProperty("level", {pos.level});
    value.setProperty("direction", {component::directionToString(pos.direction)});
    return value;
}

namespace
{
Value makePosition(system::Systems& systems, bl::entity::Entity e) {
    const component::Position* pos =
        systems.engine().entities().getComponent<component::Position>(e);
    if (pos) { return BaseFunctions::makePosition(*pos); }
    return {};
}

void getPlayer(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value& player) {
    player = systems.player().player();
    player.setProperty("name", {systems.player().name()});
    player.setProperty("gender",
                       {systems.player().gender() == player::Gender::Boy ? "boy" : "girl"});
    player.setProperty("money", {systems.player().money()});
    player.setProperty("position", makePosition(systems, systems.player().player()));

    std::vector<player::Bag::Item> items;
    systems.player().bag().getAll(items);
    player.setProperty("bag", {bl::script::ArrayValue{}});
    auto& bag = player.getProperty("bag", false).deref().value().getAsArray();
    bag.reserve(items.size());
    for (const player::Bag::Item& item : items) {
        bag.emplace_back(new Value(item.id));
        bag.back().setProperty("id", {item.id});
        bag.back().setProperty("name", {item::Item::getName(item.id)});
        bag.back().setProperty("qty", {item.qty});
    }
}

void giveItem(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value&) {
    Value::validateArgs<PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TBool,
                        PrimitiveValue::TBool>("giveItem", args);

    const unsigned int rawId = static_cast<unsigned int>(args[0].value().getAsInt());
    const item::Id item      = item::Item::cast(rawId);
    if (item != item::Id::Unknown) {
        const long qty = args[1].value().getAsInt();
        if (qty > 0) {
            systems.player().bag().addItem(item, qty);
            if (args[2].value().getAsBool()) {
                bl::util::Waiter waiter;
                system::HUD::Callback unlock = [](const std::string&) {};
                if (args[3].value().getAsBool())
                    unlock = [&waiter](const std::string&) { waiter.unblock(); };

                const std::string msg = qty > 1 ? ("Received " + std::to_string(qty) + " " +
                                                   item::Item::getName(item) + "s") :
                                                  ("Received a " + item::Item::getName(item));
                systems.hud().displayMessage(msg, unlock);
                if (args[3].value().getAsBool()) table.waitOn(waiter);
            }
        }
        else {
            BL_LOG_WARN << "qty must be a positive integer";
        }
    }
    else {
        BL_LOG_WARN << "Unknown item id: " << rawId;
    }
}

void giveMoney(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
               Value&) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TBool, PrimitiveValue::TBool>(
        "giveMoney", args);

    const long money = args[0].value().getAsInt();
    if (money > 0) {
        systems.player().money() += money;
        if (args[1].value().getAsBool()) {
            bl::util::Waiter waiter;
            system::HUD::Callback unlock = [](const std::string&) {};
            if (args[2].value().getAsBool())
                unlock = [&waiter](const std::string&) { waiter.unblock(); };

            const std::string msg = "Received " + std::to_string(money) + " monies";
            systems.hud().displayMessage(msg, unlock);
            if (args[2].value().getAsBool()) table.waitOn(waiter);
        }
    }
    else {
        BL_LOG_WARN << "qty must be a positive integer";
    }
}

void takeItem(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
              Value& result) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TInteger, PrimitiveValue::TBool>(
        "takeItem", args);

    const unsigned int rawId = static_cast<unsigned int>(args[0].value().getAsInt());
    const item::Id item      = item::Item::cast(rawId);
    if (item != item::Id::Unknown) {
        const long qty = args[1].value().getAsInt();
        if (qty > 0) {
            if (args[2].value().getAsBool()) {
                bl::util::Waiter waiter;
                std::string choice;
                system::HUD::Callback unlock = [&waiter, &choice](const std::string& c) {
                    choice = c;
                    waiter.unblock();
                };

                const std::string msg =
                    qty > 1 ?
                        ("Give " + std::to_string(qty) + " " + item::Item::getName(item) + "s?") :
                        ("Give the " + item::Item::getName(item) + "?");
                systems.hud().promptUser(msg, {"Yes", "No"}, unlock);
                table.waitOn(waiter);
                if (choice == "No") { result = false; }
            }
            else {
                result = systems.player().bag().removeItem(item, qty);
            }
        }
        else {
            BL_LOG_WARN << "qty must be a positive integer";
            result = false;
        }
    }
    else {
        BL_LOG_WARN << "Unknown item id: " << rawId;
        result = false;
    }
}

void takeMoney(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
               Value& result) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TBool>("takeMoney", args);

    const long qty = args[0].value().getAsInt();
    if (qty > 0) {
        if (args[1].value().getAsBool()) {
            bl::util::Waiter waiter;
            std::string choice;
            system::HUD::Callback unlock = [&waiter, &choice](const std::string& c) {
                choice = c;
                waiter.unblock();
            };

            const std::string msg = "Give " + std::to_string(qty) + " monies?";
            systems.hud().promptUser(msg, {"Yes", "No"}, unlock);
            table.waitOn(waiter);
            if (choice == "No") { result = false; }
        }
        if (systems.player().money() >= qty) {
            systems.player().money() -= qty;
            result = true;
        }
        else {
            result = false;
        }
    }
    else {
        BL_LOG_WARN << "qty must be a positive integer";
        result = false;
    }
}

void givePeoplemon(system::Systems&, SymbolTable&, const std::vector<Value>&, Value& result) {
    // TODO - track and give peoplemon
    result = false;
}

void takePeoplemon(system::Systems&, SymbolTable&, const std::vector<Value>&, Value& result) {
    // TODO - track and take peoplemon
    result = false;
}

void whiteout(system::Systems&, SymbolTable&, const std::vector<Value>&, Value& result) {
    // TODO - implement whiting out
    result = false;
}

void restorePeoplemon(system::Systems& systems, SymbolTable&, const std::vector<Value>&,
                      Value& result) {
    systems.player().healPeoplemon();
    result = true;
}

void displayMessage(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                    Value&) {
    Value::validateArgs<PrimitiveValue::TString, PrimitiveValue::TBool>("displayMessage", args);

    bl::util::Waiter waiter;
    system::HUD::Callback unlock = [](const std::string&) {};
    if (args[1].value().getAsBool()) unlock = [&waiter](const std::string&) { waiter.unblock(); };

    systems.hud().displayMessage(args[0].value().getAsString(), unlock);
    if (args[1].value().getAsBool()) table.waitOn(waiter);
}

void promptPlayer(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                  Value& result) {
    Value::validateArgs<PrimitiveValue::TString, PrimitiveValue::TArray>("promptPlayer", args);

    const auto& rawChoices = args[1].value().getAsArray();
    std::vector<std::string> choices;
    choices.reserve(rawChoices.size());
    for (const Value& val : rawChoices) {
        if (val.value().getType() != PrimitiveValue::TString) {
            throw Error("All choices in promptPlayer() must be String type");
        }
        choices.emplace_back(val.value().getAsString());
    }

    bl::util::Waiter waiter;
    std::string choice;
    const auto cb = [&waiter, &choice](const std::string& c) {
        choice = c;
        waiter.unblock();
    };
    systems.hud().promptUser(args[0].value().getAsString(), choices, cb);
    table.waitOn(waiter);

    result = choice;
}

void rollCredits(system::Systems&, SymbolTable&, const std::vector<Value>&, Value& result) {
    // TODO - implement credits
    result = false;
}

void getNpc(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value& npc) {
    Value::validateArgs<PrimitiveValue::TString>("getNpc", args);

    const std::string name = args[0].value().getAsString();
    const auto npcMap      = systems.engine()
                            .entities()
                            .getEntitiesWithComponents<component::NPC, component::Position>()
                            ->results();
    for (const auto& pair : npcMap) {
        if (pair.second.get<component::NPC>()->name() == name) {
            npc = pair.first;
            npc.setProperty("name", {name});
            npc.setProperty("talkedTo", {systems.interaction().npcTalkedTo(name)});
            npc.setProperty("defeated", {false});
            npc.setProperty("position",
                            BaseFunctions::makePosition(*pair.second.get<component::Position>()));
            return;
        }
    }

    npc = false;
}

void loadCharacter(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                   Value& character) {
    Value::validateArgs<PrimitiveValue::TInteger>("loadCharacter", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    if (systems.engine().entities().entityExists(entity)) {
        const component::Position* pos =
            systems.engine().entities().getComponent<component::Position>(entity);
        if (pos) {
            character = entity;
            character.setProperty("position", BaseFunctions::makePosition(*pos));

            const component::NPC* npc =
                systems.engine().entities().getComponent<component::NPC>(entity);
            if (npc) {
                character.setProperty("name", {npc->name()});
                character.setProperty("talkedTo", {systems.interaction().npcTalkedTo(npc->name())});
                character.setProperty("defeated", {false});
                return;
            }

            const component::Trainer* trainer =
                systems.engine().entities().getComponent<component::Trainer>(entity);
            if (trainer) {
                character.setProperty("name", {trainer->name()});
                character.setProperty("talkedTo",
                                      {systems.interaction().trainerTalkedto(trainer->name())});
                character.setProperty("defeated", {false}); // TODO - track defeated
                return;
            }
        }
    }

    character = false;
}

void spawnCharacter(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                    Value& result) {
    Value::validateArgs<PrimitiveValue::TString,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TString>("spawnCharacter", args);

    const map::CharacterSpawn spawn(
        component::Position(args[1].value().getAsInt(),
                            {static_cast<int>(args[2].value().getAsInt()),
                             static_cast<int>(args[3].value().getAsInt())},
                            component::directionFromString(args[4].value().getAsString())),
        args[0].value().getAsString());
    result = systems.entity().spawnCharacter(spawn);
}

void getTrainer(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                Value& trainer) {
    Value::validateArgs<PrimitiveValue::TString>("getTrainer", args);

    const std::string name = args[0].value().getAsString();
    const auto trainerMap =
        systems.engine()
            .entities()
            .getEntitiesWithComponents<component::Trainer, component::Position>()
            ->results();
    for (const auto& pair : trainerMap) {
        if (pair.second.get<component::Trainer>()->name() == name) {
            trainer = pair.first;
            trainer.setProperty("name", {name});
            trainer.setProperty("talkedTo", {systems.interaction().trainerTalkedto(name)});
            trainer.setProperty("defeated", {false}); // TODO - track who defeated
            trainer.setProperty(
                "position", BaseFunctions::makePosition(*pair.second.get<component::Position>()));
            return;
        }
    }

    trainer = false;
}

void moveEntity(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                Value& res) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TString, PrimitiveValue::TBool>(
        "moveEntity", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    const component::Direction dir  = component::directionFromString(args[1].value().getAsString());
    const bool block                = args[2].value().getAsBool();
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

    res = result;
}

void rotateEntity(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TString>("rotateEntity", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    const component::Direction dir  = component::directionFromString(args[1].value().getAsString());
    component::Position* pos =
        systems.engine().entities().getComponent<component::Position>(entity);
    if (pos && pos->direction != dir) pos->direction = dir;
}

void removeEntity(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                  Value& res) {
    Value::validateArgs<PrimitiveValue::TInteger>("removeEntity", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    if (entity != systems.player().player()) {
        const bool result = systems.engine().entities().entityExists(entity);
        systems.engine().entities().destroyEntity(entity);
        res = result;
    }
    else {
        res = false;
    }
}

void entityToPosition(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                      Value& result) {
    Value::validateArgs<PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TBool>("entityToPosition", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    component::Position* pos =
        systems.engine().entities().getComponent<component::Position>(entity);
    if (pos) {
        BL_LOG_INFO << "moved entity";
        const component::Position old = *pos;
        component::Position np        = *pos;
        np.setTiles(sf::Vector2i(args[2].value().getAsInt(), args[3].value().getAsInt()));
        np.level = args[1].value().getAsInt();
        *pos     = np;
        systems.engine().eventBus().dispatch<event::EntityMoved>({entity, old, np});
    }
    else {
        BL_LOG_ERROR << "Moving entity with no position component: " << entity;
    }

    // TODO - implement path finder
    result = true;
}

void entityInteract(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                    Value& result) {
    Value::validateArgs<PrimitiveValue::TInteger>("entityInteract", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    result                          = systems.interaction().interact(entity);
}

void setEntityLock(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TBool>("setEntityLock", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    const bool lock                 = args[1].value().getAsBool();
    systems.controllable().setEntityLocked(entity, lock, true);
}

void resetEntityLock(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                     Value&) {
    Value::validateArgs<PrimitiveValue::TInteger>("resetEntityLock", args);

    const bl::entity::Entity entity = static_cast<bl::entity::Entity>(args[0].value().getAsInt());
    systems.controllable().resetEntityLock(entity);
}

void getClock(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value& clock) {
    const system::Clock::Time now = systems.clock().now();
    clock                         = now.hour * 60 + now.minute;
    clock.setProperty("minute", {now.minute});
    clock.setProperty("hour", {now.hour});
    clock.setProperty("day", {now.day});
}

class ClockTrigger : public bl::event::Listener<event::TimeChange> {
public:
    using Callback = std::function<void()>;
    ClockTrigger(Callback cb, const system::Clock::Time& time)
    : time(time)
    , cb(cb) {}

    virtual void observe(const event::TimeChange& event) override {
        if (event.newTime.hour == time.hour && event.newTime.minute == time.minute) cb();
    }

private:
    const system::Clock::Time time;
    const Callback cb;
};

system::Clock::Time parseTime(const Value& val) {
    system::Clock::Time t(12, 0, 0);
    const PrimitiveValue& h = val.getProperty("hour", false).deref().value();
    t.hour                  = static_cast<unsigned int>(h.getNumAsInt());
    const PrimitiveValue& m = val.getProperty("minute", false).deref().value();
    t.minute                = static_cast<unsigned int>(m.getNumAsInt());
    return t;
}

void makeTime(system::Systems&, SymbolTable&, const std::vector<Value>& args, Value& time) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TInteger>("makeTime", args);
    if (args[0].value().getAsInt() < 0 || args[0].value().getAsInt() > 23)
        throw Error("hour must be in range [0, 23]");
    if (args[1].value().getAsInt() < 0 || args[1].value().getAsInt() > 59)
        throw Error("minute must be in range [0, 59]");

    const unsigned int hour   = static_cast<unsigned int>(args[0].value().getAsInt());
    const unsigned int minute = static_cast<unsigned int>(args[1].value().getAsInt());
    time                      = hour * 60 + minute;
    time.setProperty("hour", {hour});
    time.setProperty("minute", {minute});
}

void waitUntilTime(system::Systems& systems, SymbolTable& table, const std::vector<Value>& args,
                   Value&) {
    Value::validateArgs<PrimitiveValue::TAny, PrimitiveValue::TBool>("waitUntilTime", args);

    const system::Clock::Time now  = systems.clock().now();
    const system::Clock::Time then = parseTime(args[0]);
    const unsigned int nts         = now.hour * 60 + now.minute;
    const unsigned int ets         = then.hour * 60 + then.minute;
    if (nts < ets || (nts != ets && args[1].value().getAsBool())) {
        bl::util::Waiter waiter;
        const auto unlock = [&waiter]() { waiter.unblock(); };
        ClockTrigger trigger(unlock, then);
        bl::event::ClassGuard<event::TimeChange> guard(&trigger);
        guard.subscribe(systems.engine().eventBus());
        table.waitOn(waiter);
    }
}

void runAtClockTime(system::Systems&, SymbolTable& table, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TAny, PrimitiveValue::TInteger>("runAtClockTime", args);

    const std::string source       = args[0].value().getAsString();
    const system::Clock::Time time = parseTime(args[1]);

    // trick to avoid having to create a thread here. hacky but cleaner
    const std::string program = "waitUntilTime(makeTime(" + std::to_string(time.hour) + ", " +
                                std::to_string(time.minute) + "), true);\nrun(\"" + source +
                                "\", false);";

    bl::script::Script script(program, table);
    if (!script.valid()) throw Error("Syntax error in script passed to runAtClockTime()");
    script.runBackground(table.manager());
}

void addSaveEntry(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                  Value& result) {
    Value::validateArgs<PrimitiveValue::TString, EntryTypes>("addSaveEntry", args);
    systems.scripts().setEntry(args[0].value().getAsString(), args[1].value());
    result = true;
}

void getSaveEntry(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                  Value& result) {
    Value::validateArgs<PrimitiveValue::TString>("getSaveEntry", args);
    const Value* val = systems.scripts().getEntry(args[0].value().getAsString());
    result           = val ? *val : false;
}

void loadMap(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TString, PrimitiveValue::TInteger>("loadMap", args);
    systems.engine().eventBus().dispatch<event::SwitchMapTriggered>(
        {args[0].value().getAsString(), static_cast<int>(args[1].value().getAsInt())});
}

void checkConvFlag(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                   Value& result) {
    Value::validateArgs<PrimitiveValue::TString>("checkConvFlag", args);
    result = systems.interaction().flagSet(args[0].value().getAsString());
}

void setConvFlag(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TString>("setConvFlag", args);
    systems.interaction().setFlag(args[0].value().getAsString());
}

void setAmbientLight(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                     Value&) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TInteger, PrimitiveValue::TBool>(
        "setAmbientLight", args);
    if (args[0].value().getAsInt() < 0) { throw Error("Light level must be positive"); }
    if (args[0].value().getAsInt() > 255) { throw Error("Light level must be under 255"); }

    const std::uint8_t low  = static_cast<std::uint8_t>(args[0].value().getAsInt());
    const std::uint8_t high = static_cast<std::uint8_t>(args[1].value().getAsInt());
    const bool sunlight     = args[2].value().getAsBool();
    systems.world().activeMap().lightingSystem().setAmbientLevel(low, high);
    systems.world().activeMap().lightingSystem().adjustForSunlight(sunlight);
}

void createLight(system::Systems& systems, SymbolTable&, const std::vector<Value>& args,
                 Value& result) {
    Value::validateArgs<PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger>("createLight", args);

    const float x = args[0].value().getAsInt();
    const float y = args[1].value().getAsInt();
    const float r = args[2].value().getAsInt();

    if (x < 0.f || x >= systems.world().activeMap().sizePixels().x) {
        throw Error("Light x coordinate is out of range: " + std::to_string(x));
    }
    if (y < 0.f || y >= systems.world().activeMap().sizePixels().y) {
        throw Error("Light y coordinate is out of range: " + std::to_string(y));
    }
    if (r < 0.f) { throw Error("Light radius must be positive"); }

    const map::Light light(static_cast<std::uint16_t>(r),
                           {static_cast<int>(x), static_cast<int>(y)});
    const map::LightingSystem::Handle handle =
        systems.world().activeMap().lightingSystem().addLight(light);

    result = handle;
}

void updateLight(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger,
                        PrimitiveValue::TInteger>("updateLight", args);

    const map::LightingSystem::Handle handle =
        static_cast<map::LightingSystem::Handle>(args[0].value().getAsInt());
    const float x = args[1].value().getAsInt();
    const float y = args[2].value().getAsInt();
    const float r = args[3].value().getAsInt();

    if (x < 0.f || x >= systems.world().activeMap().sizePixels().x) {
        throw Error("Light x coordinate is out of range: " + std::to_string(x));
    }
    if (y < 0.f || y >= systems.world().activeMap().sizePixels().y) {
        throw Error("Light y coordinate is out of range: " + std::to_string(y));
    }
    if (r < 0.f) { throw Error("Light radius must be positive"); }

    const map::Light light(static_cast<std::uint16_t>(r),
                           {static_cast<int>(x), static_cast<int>(y)});
    systems.world().activeMap().lightingSystem().updateLight(handle, light);
}

void removeLight(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TInteger>("removeLight", args);
    const map::LightingSystem::Handle handle =
        static_cast<map::LightingSystem::Handle>(args[0].value().getAsInt());
    systems.world().activeMap().lightingSystem().removeLight(handle);
}

void clearWeather(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value&) {
    systems.world().activeMap().weatherSystem().set(map::Weather::None);
}

void makeRain(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TBool, PrimitiveValue::TBool>("makeRain", args);

    map::Weather::Type t;
    if (args[0].value().getAsBool()) {
        if (args[1].value().getAsBool())
            t = map::Weather::LightRainThunder;
        else
            t = map::Weather::LightRain;
    }
    else {
        if (args[1].value().getAsBool())
            t = map::Weather::HardRainThunder;
        else
            t = map::Weather::HardRain;
    }

    systems.world().activeMap().weatherSystem().set(t);
}

void makeSnow(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TBool, PrimitiveValue::TBool>("makeSnow", args);

    map::Weather::Type t;
    if (args[0].value().getAsBool()) {
        if (args[1].value().getAsBool())
            t = map::Weather::LightSnowThunder;
        else
            t = map::Weather::LightSnow;
    }
    else {
        if (args[1].value().getAsBool())
            t = map::Weather::HardSnowThunder;
        else
            t = map::Weather::HardSnow;
    }

    systems.world().activeMap().weatherSystem().set(t);
}

void makeSunny(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value&) {
    systems.world().activeMap().weatherSystem().set(map::Weather::Sunny);
}

void makeSandstorm(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value&) {
    systems.world().activeMap().weatherSystem().set(map::Weather::SandStorm);
}

void makeFog(system::Systems& systems, SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TBool>("makeFog", args);

    const map::Weather::Type t =
        args[0].value().getAsBool() ? map::Weather::ThickFog : map::Weather::ThinFog;
    systems.world().activeMap().weatherSystem().set(t);
}

void makeRandomRain(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value&) {
    systems.world().activeMap().weatherSystem().set(map::Weather::WaterRandom);
}

void makeRandomSnow(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value&) {
    systems.world().activeMap().weatherSystem().set(map::Weather::SnowRandom);
}

void makeRandomDesert(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value&) {
    systems.world().activeMap().weatherSystem().set(map::Weather::DesertRandom);
}

void makeRandomWeather(system::Systems& systems, SymbolTable&, const std::vector<Value>&, Value&) {
    systems.world().activeMap().weatherSystem().set(map::Weather::AllRandom);
}

void getCurrentWeather(system::Systems& systems, SymbolTable&, const std::vector<Value>&,
                       Value& result) {
    using T = map::Weather::Type;

    switch (systems.world().activeMap().weatherSystem().getType()) {
    case T::LightRain:
    case T::LightRainThunder:
    case T::HardRain:
    case T::HardRainThunder:
        result = "rain";
        break;
    case T::LightSnow:
    case T::LightSnowThunder:
    case T::HardSnow:
    case T::HardSnowThunder:
        result = "snow";
        break;
    case T::ThinFog:
    case T::ThickFog:
        result = "fog";
        break;
    case T::Sunny:
        result = "sunny";
        break;
    case T::SandStorm:
        result = "sandstorm";
        break;
    default:
        result = "none";
        break;
    }
}

} // namespace

} // namespace script
} // namespace core
