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
     * @brief Returns the name of the battler
     *
     */
    virtual const std::string& name() const override;

    /**
     * @brief Destroy the AIController object
     *
     */
    virtual ~AIController() = default;

private:
    const std::string _name;
    std::vector<item::Id> items; // TODO - copy items here from the trainer

    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon() override;
};

} // namespace battle
} // namespace core

#endif
