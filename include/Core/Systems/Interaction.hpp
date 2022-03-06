#ifndef CORE_SYSTEMS_INTERACTION_HPP
#define CORE_SYSTEMS_INTERACTION_HPP

#include <BLIB/Entities.hpp>
#include <BLIB/Events.hpp>
#include <Core/AI/Conversation.hpp>
#include <Core/Components/Trainer.hpp>
#include <Core/Events/Battle.hpp>
#include <Core/Events/GameSave.hpp>
#include <unordered_map>
#include <unordered_set>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief The main interaction system. Manages interaction between entities. Handles item logic and
 *        NPC conversations followed by trainer battles if applicable
 *
 * @ingroup Systems
 *
 */
class Interaction
: public bl::event::Listener<event::GameSaveInitializing, event::BattleCompleted> {
public:
    /**
     * @brief Construct a new Interaction system
     *
     * @param owner The primary Systems object
     */
    Interaction(Systems& owner);

    /**
     * @brief Subscribes to the game event bus
     *
     */
    void init();

    /**
     * @brief Performs an interation on behalf of the given entity
     *
     * @param interactor The entity doing the interaction
     * @return True if an interaction occurred, false if nothing happened
     */
    bool interact(bl::entity::Entity interactor);

    /**
     * @brief Returns whether or not the given npc has been talked to
     *
     * @param name The name of the NPC
     * @return True if talked to in the current map, false otherwise
     */
    bool npcTalkedTo(const std::string& name) const;

    /**
     * @brief Returns whether or not the given trainer has been talked to
     *
     * @param name The name of the trainer
     * @return True if talked to in the current map, false otherwise
     */
    bool trainerTalkedto(const std::string& name) const;

    /**
     * @brief Checks if the given conversation flag has been set
     *
     * @param flag The name of the flag to check
     * @return True if the flag is set, false otherwise
     */
    bool flagSet(const std::string& flag) const;

    /**
     * @brief Sets the conversation flag
     *
     * @param flag The name of the flag to set
     */
    void setFlag(const std::string& flag);

private:
    Systems& owner;
    bl::entity::Entity interactingEntity;
    ai::Conversation currentConversation;
    std::unordered_map<std::string, std::unordered_set<std::string>> talkedTo;
    std::unordered_set<std::string> flags;
    component::Trainer const* interactingTrainer;

    void processConversationNode();
    void faceEntity(bl::entity::Entity toRotate, bl::entity::Entity toFace);

    void continuePressed();
    void failMessageFinished();
    void choiceMade(const std::string& choice);
    void giveItemDecided(const std::string& choice);
    void giveMoneyDecided(const std::string& choice);

    virtual void observe(const event::GameSaveInitializing& save) override;
    virtual void observe(const event::BattleCompleted& battle) override;
    void setTalked(const std::string& name);

    void startBattle();

    friend struct bl::serial::json::SerializableObject<Interaction>;
};

} // namespace system
} // namespace core

#endif
