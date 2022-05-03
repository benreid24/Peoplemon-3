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

private:
    virtual void startChooseAction() override;
    virtual void startChoosePeoplemon(bool fromFaint, bool reviveOnly) override;
};

} // namespace battle
} // namespace core

#endif
