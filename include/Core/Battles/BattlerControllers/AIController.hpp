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
     * @brief Construct a new AIController with no items
     *
     */
    AIController() = default;

    /**
     * @brief Construct a new AIController with the given items
     *
     * @param items The items that may be used in battle
     */
    AIController(const std::vector<item::Id>& items);

    /**
     * @brief Destroy the AIController object
     *
     */
    virtual ~AIController() = default;

private:
    std::vector<item::Id> items; // TODO - copy items here from the trainer

    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon() override;
};

} // namespace battle
} // namespace core

#endif
