#ifndef GAME_BATTLES_BATTLECONTROLLERS_PLAYERCONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLERS_PLAYERCONTROLLER_HPP

#include <Core/Battles/BattlerControllers/BattlerController.hpp>

namespace core
{
namespace battle
{
/**
 * @brief This controller allows the player to control a battler
 *
 * @ingroup Battles
 *
 */
class PlayerController : public BattlerController {
public:
    // TODO - how to interface with menus in the view? dont forget to remove items from the bag

    /**
     * @brief Destroy the Player Controller object
     *
     */
    virtual ~PlayerController() = default;

    /**
     * @brief Returns the name of the battler
     *
     */
    virtual const std::string& name() const override;

private:
    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon() override;
};

} // namespace battle
} // namespace core

#endif
