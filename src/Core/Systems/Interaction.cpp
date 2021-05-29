#include <Core/Systems/Interaction.hpp>

#include <Core/Components/Item.hpp>
#include <Core/Components/NPC.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Events/Item.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Interaction::Interaction(Systems& owner)
: owner(owner)
, interactingEntity(bl::entity::InvalidEntity)
, currentConversation(owner) {}

bool Interaction::interact(bl::entity::Entity interactor) {
    const component::Position* pos =
        owner.engine().entities().getComponent<component::Position>(interactor);
    if (!pos) return false;

    const bl::entity::Entity interacted = owner.position().search(*pos, pos->direction, 1);
    if (interacted == bl::entity::InvalidEntity) return false;
    BL_LOG_INFO << "Entity " << interactor << " interacted with entity: " << interacted;

    if (interactor != owner.player().player() && interacted != owner.player().player()) {
        BL_LOG_WARN << "Nonplayer entity " << interactor << " interacted with " << interacted;
        return false;
    }
    const bl::entity::Entity nonplayer =
        interactor != owner.player().player() ? interactor : interacted;

    const component::NPC* npc = owner.engine().entities().getComponent<component::NPC>(nonplayer);
    if (npc) {
        owner.controllable().setEntityLocked(nonplayer, true);
        faceEntity(nonplayer, owner.player().player());
        faceEntity(owner.player().player(), nonplayer);
        currentConversation.setConversation(npc->conversation(), nonplayer);
        interactingEntity = nonplayer;
        processConversationNode();
    }
    else {
        const component::Trainer* trainer =
            owner.engine().entities().getComponent<component::Trainer>(nonplayer);
        if (trainer) {
            owner.controllable().setEntityLocked(nonplayer, true);
            faceEntity(nonplayer, owner.player().player());
            faceEntity(owner.player().player(), nonplayer);
            // TODO - handle battle
            currentConversation.setConversation(trainer->beforeBattleConversation(), nonplayer);
            interactingEntity = nonplayer;
            processConversationNode();
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

    return false;
}

void Interaction::processConversationNode() {
    if (interactingEntity == bl::entity::InvalidEntity) return;
    if (currentConversation.finished()) {
        owner.controllable().resetEntityLock(interactingEntity);
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
        owner.player().bag().addItem(node.item());
        // TODO - add prefix to item metadata for a/the use
        owner.hud().displayMessage("Got a " + item::Item::getName(node.item()),
                                   std::bind(&Interaction::continuePressed, this));
        break;

    case E::TakeItem:
        owner.hud().promptUser(
            "Hand over a " + item::Item::getName(node.item()),
            {"Yes", "No"},
            std::bind(&Interaction::giveItemDecided, this, std::placeholders::_1));
        break;

    case E::GiveMoney:
        // TODO - track and add player money
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
        if (owner.player().bag().removeItem(currentConversation.currentNode().item())) {
            currentConversation.notifyCheckPassed();
            processConversationNode();
        }
        else {
            owner.hud().displayMessage(
                "You don't have a single " +
                    item::Item::getName(currentConversation.currentNode().item()) + "!",
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
        if (true) { // TODO - track and take player money
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

} // namespace system
} // namespace core
