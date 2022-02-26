#ifndef GAME_BATTLES_BATTLECONTROLLERS_REMOTECONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLERS_REMOTECONTROLLER_HPP

#include <Game/Battles/BattlerControllers/BattlerController.hpp>

namespace game
{
namespace battle
{
/**
 * @brief This controller receives player commands from over the network
 *
 * @ingroup Battles
 *
 */
class PlayerController : public BattlerController {
public:
    // TODO - how to interface with menus in the view?

private:
    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon() override;
};

} // namespace battle
} // namespace game

#endif
