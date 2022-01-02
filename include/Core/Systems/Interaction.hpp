#ifndef CORE_SYSTEMS_INTERACTION_HPP
#define CORE_SYSTEMS_INTERACTION_HPP

#include <BLIB/Entities.hpp>
#include <BLIB/Events.hpp>
#include <Core/AI/Conversation.hpp>
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
class Interaction : public bl::event::Listener<event::GameLoading, event::GameSaving> {
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

private:
    Systems& owner;
    bl::entity::Entity interactingEntity;
    ai::Conversation currentConversation;
    std::unordered_map<std::string, std::unordered_set<std::string>> talkedTo;
    // TODO - data for battle transition if talking to fightable trainer?

    void processConversationNode();
    void faceEntity(bl::entity::Entity toRotate, bl::entity::Entity toFace);

    void continuePressed();
    void failMessageFinished();
    void choiceMade(const std::string& choice);
    void giveItemDecided(const std::string& choice);
    void giveMoneyDecided(const std::string& choice);

    virtual void observe(const event::GameSaving& save) override;
    virtual void observe(const event::GameLoading& save) override;
    void setTalked(const std::string& name);

    friend struct bl::serial::json::SerializableObject<Interaction>;
};

} // namespace system
} // namespace core

namespace bl
{
namespace serial
{
namespace json
{
template<>
struct SerializableObject<core::system::Interaction> : public SerializableObjectBase {
    using I = core::system::Interaction;
    using M = std::unordered_map<std::string, std::unordered_set<std::string>>;

    SerializableField<I, M> talkedTo;

    SerializableObject()
    : talkedTo("talked", *this, &I::talkedTo) {}
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
