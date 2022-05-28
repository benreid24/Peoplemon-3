#ifndef GAME_BATTLES_BATTLECONTROLLERS_REMOTECONTROLLER_HPP
#define GAME_BATTLES_BATTLECONTROLLERS_REMOTECONTROLLER_HPP

#include <Core/Battles/BattlerControllers/BattlerController.hpp>

namespace core
{
namespace battle
{
/**
 * @brief This controller receives player commands from over the network
 *
 * @ingroup Battles
 *
 */
class RemoteController : public BattlerController {
public:
    // TODO - how to interface with menus in the view?

    /**
     * @brief Destroy the Remote Controller object
     *
     */
    virtual ~RemoteController() = default;

    /**
     * @brief Returns the name of the battler
     *
     */
    virtual const std::string& name() const override;

    /**
     * @brief Does nothing
     *
     */
    virtual void refresh() override;

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
    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon(bool fromFaint, bool reviveOnly) override;
    virtual void startChooseToContinue() override;
};

} // namespace battle
} // namespace core

#endif
