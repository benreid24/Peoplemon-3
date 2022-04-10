#ifndef CORE_BATTLES_LOCALBATTLECONTROLLER_HPP
#define CORE_BATTLES_LOCALBATTLECONTROLLER_HPP

#include <Core/Battles/BattleControllers/BattleController.hpp>
#include <Core/Battles/BattleState.hpp>

namespace core
{
namespace battle
{
/**
 * @brief Battle controller for local battles against peoplemon or trainers
 *
 * @ingroup Battle
 *
 */
class LocalBattleController : public BattleController {
public:
    /**
     * @brief Construct a new Local Battle Controller
     *
     */
    LocalBattleController();

    /**
     * @brief Destroy the Local Battle Controller
     *
     */
    virtual ~LocalBattleController() = default;

protected:
    /**
     * @brief Updates the battle state based on the previous state, view state, and queue state
     *
     * @param viewSynced Whether or not the view is up-to-date and static
     * @param queueEmpty Whether or not the command queue has been emptied fully
     */
    void updateBattleState(bool viewSynced, bool queueEmpty);

private:
    bool currentStageInitialized;

    virtual void onCommandQueued(const Command& cmd) override;
    virtual void onCommandProcessed(const Command& cmd) override;
    virtual void onUpdate(bool viewSynced, bool queueEmpty) override;

    void setBattleState(BattleState::Stage nextState);
    void initCurrentStage();
    void checkCurrentStage(bool viewSynced, bool queueEmpty);
};

} // namespace battle
} // namespace core

#endif
