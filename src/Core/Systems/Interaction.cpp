#include <Core/Systems/Interaction.hpp>

#include <BLIB/Serialization/JSON.hpp>
#include <Core/Battles/BattlerControllers/AIController.hpp>
#include <Core/Components/Item.hpp>
#include <Core/Components/NPC.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Events/Item.hpp>
#include <Core/Files/GameSave.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
std::string trainerName(const std::string& n) { return "tnr:" + n; }

std::string npcName(const std::string& n) { return "npc:" + n; }

} // namespace

Interaction::Interaction(Systems& owner)
: owner(owner)
, interactingEntity(bl::entity::InvalidEntity)
, currentConversation(owner)
, interactingTrainer(nullptr) {}

void Interaction::init() { owner.engine().eventBus().subscribe(this); }

bool Interaction::interact(bl::entity::Entity interactor) {
    const component::Position* pos =
        owner.engine().entities().getComponent<component::Position>(interactor);
    if (!pos) return false;

    const bl::entity::Entity interacted = owner.position().search(*pos, pos->direction, 1);
    if (interacted != bl::entity::InvalidEntity) {
        BL_LOG_INFO << "Entity " << interactor << " interacted with entity: " << interacted;

        if (interactor != owner.player().player() && interacted != owner.player().player()) {
            BL_LOG_WARN << "Nonplayer entity " << interactor << " interacted with " << interacted;
            return false;
        }
        const bl::entity::Entity nonplayer =
            interactor != owner.player().player() ? interactor : interacted;

        const component::NPC* npc =
            owner.engine().entities().getComponent<component::NPC>(nonplayer);
        if (npc) {
            setTalked(npcName(npc->name()));
            owner.controllable().setEntityLocked(nonplayer, true);
            faceEntity(nonplayer, owner.player().player());
            faceEntity(owner.player().player(), nonplayer);
            currentConversation.setConversation(
                npc->conversation(), nonplayer, npcTalkedTo(npc->name()));
            interactingEntity = nonplayer;
            processConversationNode();
            return true;
        }
        else {
            const component::Trainer* const trainer =
                owner.engine().entities().getComponent<component::Trainer>(nonplayer);
            if (trainer) {
                setTalked(trainerName(trainer->name()));
                owner.controllable().setEntityLocked(nonplayer, true);
                faceEntity(nonplayer, owner.player().player());
                faceEntity(owner.player().player(), nonplayer);
                interactingTrainer = trainer;
                currentConversation.setConversation(trainer->beforeBattleConversation(),
                                                    nonplayer,
                                                    trainerTalkedto(trainer->name()));
                interactingEntity = nonplayer;
                processConversationNode();
                return true;
            }
        }

        // Check for item if player
        if (interactor == owner.player().player()) {
            const component::Item* ic =
                owner.engine().entities().getComponent<component::Item>(interacted);
            if (ic) {
                const std::string name = item::Item::getName(ic->id());
                BL_LOG_INFO << "Player picked up: " << static_cast<unsigned int>(ic->id()) << " ("
                            << name << ")";
                owner.player().bag().addItem(ic->id(), 1);
                owner.engine().eventBus().dispatch<event::ItemPickedUp>({ic->id()});
                owner.engine().entities().destroyEntity(interacted);
                owner.hud().displayMessage("Picked up a " + name);
                return true;
            }
        }
    }

    // if nothing else see if map has interactable events
    return owner.world().activeMap().interact(interactor, pos->move(pos->direction));
}

void Interaction::processConversationNode() {
    if (interactingEntity == bl::entity::InvalidEntity) return;
    if (currentConversation.finished()) {
        if (interactingTrainer) { startBattle(); }
        else {
            owner.controllable().resetEntityLock(interactingEntity);
        }
        return;
    }

    using E                              = file::Conversation::Node::Type;
    const file::Conversation::Node& node = currentConversation.currentNode();
    switch (node.getType()) {
    case E::Talk:
        owner.hud().displayMessage(node.message(), std::bind(&Interaction::continuePressed, this));
        break;

    case E::Prompt: {
        std::vector<std::string> choices;
        choices.reserve(node.choices().size());
        for (const auto& pair : node.choices()) choices.push_back(pair.first);
        owner.hud().promptUser(node.message(),
                               choices,
                               std::bind(&Interaction::choiceMade, this, std::placeholders::_1));
    } break;

    case E::GiveItem:
        owner.player().bag().addItem(node.item().id);
        if (node.item().afterPrompt) {
            // TODO - add prefix to item metadata for a/the use
            owner.hud().displayMessage("Got a " + item::Item::getName(node.item().id),
                                       std::bind(&Interaction::continuePressed, this));
        }
        break;

    case E::TakeItem:
        if (node.item().beforePrompt) {
            owner.hud().promptUser(
                "Hand over a " + item::Item::getName(node.item().id),
                {"Yes", "No"},
                std::bind(&Interaction::giveItemDecided, this, std::placeholders::_1));
        }
        else {
            if (owner.player().bag().removeItem(currentConversation.currentNode().item().id)) {
                currentConversation.notifyCheckPassed();
                processConversationNode();
            }
        }
        break;

    case E::GiveMoney:
        owner.player().money() += node.money();
        owner.hud().displayMessage("Received " + std::to_string(node.money()) + " monies",
                                   std::bind(&Interaction::continuePressed, this));
        break;

    case E::TakeMoney:
        owner.hud().promptUser(
            "Fork over " + std::to_string(node.money()) + " monies?",
            {"Yes", "No"},
            std::bind(&Interaction::giveMoneyDecided, this, std::placeholders::_1));
        break;

    default:
        BL_LOG_ERROR << "Invalid conversation node type " << node.getType() << ", terminating";
        owner.controllable().resetEntityLock(interactingEntity);
        interactingEntity = bl::entity::InvalidEntity;
        break;
    }
}

void Interaction::continuePressed() {
    currentConversation.notifyNext();
    processConversationNode();
}

void Interaction::failMessageFinished() {
    currentConversation.notifyCheckFailed();
    processConversationNode();
}

void Interaction::choiceMade(const std::string& c) {
    currentConversation.notifyChoiceMade(c);
    processConversationNode();
}

void Interaction::giveItemDecided(const std::string& c) {
    if (c == "Yes") {
        if (owner.player().bag().removeItem(currentConversation.currentNode().item().id)) {
            currentConversation.notifyCheckPassed();
            processConversationNode();
        }
        else {
            owner.hud().displayMessage("A voice echos in your head: YOU DON'T HAVE THAT ITEM BRO",
                                       std::bind(&Interaction::failMessageFinished, this));
        }
    }
    else {
        currentConversation.notifyCheckFailed();
        processConversationNode();
    }
}

void Interaction::giveMoneyDecided(const std::string& c) {
    if (c == "Yes") {
        const long money = currentConversation.currentNode().money();
        if (owner.player().money() >= money) {
            owner.player().money() -= money;
            currentConversation.notifyCheckPassed();
            processConversationNode();
        }
        else {
            owner.hud().displayMessage("You don't have enough monei, get a job!",
                                       std::bind(&Interaction::failMessageFinished, this));
        }
    }
    else {
        currentConversation.notifyCheckFailed();
        processConversationNode();
    }
}

void Interaction::faceEntity(bl::entity::Entity rot, bl::entity::Entity face) {
    component::Position* mpos = owner.engine().entities().getComponent<component::Position>(rot);
    const component::Position* fpos =
        owner.engine().entities().getComponent<component::Position>(face);
    if (mpos && fpos) {
        const component::Direction dir = component::Position::facePosition(*mpos, *fpos);
        mpos->direction                = dir;
    }
}

bool Interaction::npcTalkedTo(const std::string& name) const {
    const auto wit = talkedTo.find(owner.world().activeMap().name());
    if (wit == talkedTo.end()) return false;
    return wit->second.find("npc:" + name) != wit->second.end();
}

bool Interaction::trainerTalkedto(const std::string& name) const {
    const auto wit = talkedTo.find(owner.world().activeMap().name());
    if (wit == talkedTo.end()) return false;
    return wit->second.find("tnr:" + name) != wit->second.end();
}

bool Interaction::flagSet(const std::string& name) const { return flags.find(name) != flags.end(); }

void Interaction::setFlag(const std::string& name) { flags.emplace(name); }

void Interaction::observe(const event::GameSaveInitializing& save) {
    save.gameSave.interaction.convFlags = &flags;
    save.gameSave.interaction.talkedto  = &talkedTo;
}

void Interaction::observe(const event::BattleCompleted& battle) {
    if (interactingTrainer && battle.type == battle::Battle::Type::Trainer) {
        currentConversation.setConversation(interactingTrainer->afterBattleConversation(),
                                            interactingEntity,
                                            trainerTalkedto(interactingTrainer->name()));
        processConversationNode();
        interactingTrainer = nullptr;
    }
}

void Interaction::setTalked(const std::string& name) {
    auto wit = talkedTo.find(owner.world().activeMap().name());
    if (wit == talkedTo.end()) {
        wit = talkedTo.try_emplace(owner.world().activeMap().name()).first;
    }
    wit->second.emplace(name);
}

void Interaction::startBattle() {
    std::unique_ptr<battle::Battle> battle =
        battle::Battle::create(owner.player(), battle::Battle::Type::Trainer);

    std::vector<pplmn::BattlePeoplemon> team;
    team.reserve(interactingTrainer->team().size());
    for (const auto& ppl : interactingTrainer->team()) {
        team.emplace_back(const_cast<pplmn::OwnedPeoplemon*>(&ppl));
    }
    battle->state.enemy().init(std::move(team),
                               std::make_unique<battle::AIController>(interactingTrainer->items()));
    // TODO - set battle controller to local when created

    owner.engine().eventBus().dispatch<event::BattleStarted>({std::move(battle)});
}

} // namespace system
} // namespace core
