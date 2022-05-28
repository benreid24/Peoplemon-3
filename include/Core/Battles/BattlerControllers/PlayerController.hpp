#ifndef GAME_BATTLES_BATTLECONTROLLERS_PLAYERCONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLERS_PLAYERCONTROLLER_HPP

#include <Core/Battles/BattlerControllers/BattlerController.hpp>
#include <Core/Systems/Player.hpp>

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
     * @param player The player system
     * @param menu The menu to get player choices from
     */
    PlayerController(system::Player& player, view::PlayerMenu& menu);

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

    /**
     * @brief Removes the item from the battler's inventory
     *
     * @param item The item to remove
     */
    virtual void removeItem(item::Id item) override;

    /**
     * @brief Returns true
     *
     */
    virtual bool isHost() const override;

private:
    enum struct State { Waiting, PickingTurn, PickingFaintReplacement };

    State state;
    system::Player& player;
    view::PlayerMenu& menu;

    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon(bool fromFaint, bool reviveOnly) override;
    virtual void startChooseToContinue() override;
};

} // namespace battle
} // namespace core

#endif
