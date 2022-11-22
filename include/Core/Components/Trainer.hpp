#ifndef CORE_COMPONENTS_TRAINER_HPP
#define CORE_COMPONENTS_TRAINER_HPP

#include <Core/Files/Conversation.hpp>
#include <Core/Files/Trainer.hpp>

namespace core
{
namespace component
{
/**
 * @brief Adding this to an entity will make it a trainer that can battle
 *
 * @ingroup Components
 *
 */
class Trainer {
public:
    /**
     * @brief Construct a new Trainer component
     *
     * @param trainer The trainer data to create from
     */
    Trainer(const file::Trainer& trainer);

    /**
     * @brief Returns the file the trainer was spawned from
     *
     */
    const std::string& file() const;

    /**
     * @brief The name of the NPC
     *
     */
    const std::string& name() const;

    /**
     * @brief The conversation before battle is entered
     *
     */
    const file::Conversation& beforeBattleConversation() const;

    /**
     * @brief The conversation used after battle if interacted again
     *
     */
    const file::Conversation& afterBattleConversation() const;

    /**
     * @brief Dialog line said in battle screen when battle is lost
     *
     */
    const std::string& loseBattleDialog() const;

    /**
     * @brief Returns how far the trainer can see in tiles
     *
     */
    std::uint8_t range() const;

    /**
     * @brief Returns the team of peoplemon the trainer has
     *
     */
    const std::vector<pplmn::OwnedPeoplemon>& team() const;

    /**
     * @brief Returns the items the trainer has
     *
     */
    const std::vector<item::Id>& items() const;

    /**
     * @brief Returns whether or not this trainer was defeated
     *
     */
    bool defeated() const;

    /**
     * @brief Marks this trainer as defeated
     *
     */
    void setDefeated();

    /**
     * @brief Returns the base payout of the trainer
     *
     */
    std::uint8_t basePayout() const;

    // TODO - expose battle ai

private:
    const std::string sourceFile;
    const std::string _name;
    file::Conversation beforeBattle;
    file::Conversation afterBattle;
    const std::string loseDialog;
    const std::uint8_t visionRange;
    const std::vector<pplmn::OwnedPeoplemon> peoplemon;
    const std::vector<item::Id> _items;
    std::uint8_t payout;
    bool beat;
};

} // namespace component
} // namespace core

#endif
