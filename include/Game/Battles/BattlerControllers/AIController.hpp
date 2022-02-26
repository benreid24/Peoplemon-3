#ifndef GAME_BATTLES_BATTLECONTROLLERS_AICONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLERS_AICONTROLLER_HPP

#include <Game/Battles/BattlerControllers/BattlerController.hpp>

namespace game
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

private:
    std::vector<core::item::Id> items; // TODO - copy items here from the trainer

    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon() override;
};

} // namespace battle
} // namespace game

#endif
