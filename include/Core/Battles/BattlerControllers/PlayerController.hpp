#ifndef GAME_BATTLES_BATTLECONTROLLERS_PLAYERCONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLERS_PLAYERCONTROLLER_HPP

#include <Core/Battles/BattlerControllers/BattlerController.hpp>

namespace core
{
namespace battle
{
namespace view
{
class PlayerMenu;
}

/**
 * @brief This controller allows the player to control a battler
 *
 * @ingroup Battles
 *
 */
class PlayerController : public BattlerController {
public:
    /**
     * @brief Construct a new Player Controller
     *
     * @param menu The menu to get player choices from
     */
    PlayerController(view::PlayerMenu& menu);

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

    /**
     * @brief Checks the menu state
     *
     */
    virtual void refresh() override;

private:
    enum struct State { Waiting, PickingTurn, PickingFaintReplacement };

    State state;
    view::PlayerMenu& menu;

    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon() override;
};

} // namespace battle
} // namespace core

#endif
