#ifndef GAME_BATTLES_BATTLECONTROLLERS_AICONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLERS_AICONTROLLER_HPP

#include <Core/Battles/BattlerControllers/BattlerController.hpp>

namespace core
{
namespace battle
{
/**
 * @brief This controller allows an AI to battle
 *
 * @ingroup Battles
 *
 */
class AIController : public BattlerController {
public:
    // TODO - different personalities and whatnot

    /**
     * @brief Construct a new AIController for a wild peoplemon
     *
     * @param wildId The id of the wild peoplemon
     *
     */
    AIController(pplmn::Id wildId);

    /**
     * @brief Construct a new AIController with the given items
     *
     * @param trainerName The name of the trainer being controlled
     * @param items The items that may be used in battle
     */
    AIController(const std::string& trainerName, const std::vector<item::Id>& items);

    /**
     * @brief Does nothing
     *
     */
    virtual void refresh() override;

    /**
     * @brief Returns the name of the battler
     *
     */
    virtual const std::string& name() const override;

    /**
     * @brief Destroy the AIController object
     *
     */
    virtual ~AIController() = default;

    /**
     * @brief Removes the item from the battler's inventory
     *
     * @param item The item to remove
     * @return True if the item was removed, false otherwise
     */
    virtual bool removeItem(item::Id item) override;

    /**
     * @brief Returns false
     *
     */
    virtual bool isHost() const override;

private:
    const std::string _name;
    std::vector<item::Id> items;

    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon(bool fromFaint, bool reviveOnly) override;
    virtual void startChooseToContinue() override;
};

} // namespace battle
} // namespace core

#endif
