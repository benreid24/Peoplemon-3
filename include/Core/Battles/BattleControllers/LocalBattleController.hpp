#ifndef CORE_BATTLES_LOCALBATTLECONTROLLER_HPP
#define CORE_BATTLES_LOCALBATTLECONTROLLER_HPP

#include <Core/Battles/BattleControllers/BattleController.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Peoplemon/MoveEffect.hpp>

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
    bool finalEffectsApplied;

    virtual void onCommandQueued(const Command& cmd) override;
    virtual void onCommandProcessed(const Command& cmd) override;
    virtual void onUpdate(bool viewSynced, bool queueEmpty) override;

    void setBattleState(BattleState::Stage nextState);
    void initCurrentStage();
    void checkCurrentStage(bool viewSynced, bool queueEmpty);
    BattleState::Stage getNextStage(BattleState::Stage ns);

    void startUseMove(Battler& user, int index);
    bool checkMoveCancelled(Battler& user, Battler& victim, int i, pplmn::MoveId move, int pwr,
                            pplmn::Type moveType, pplmn::MoveEffect effect,
                            bool isChargeSecondTurn);
    void applyDamageWithChecks(Battler& owner, pplmn::BattlePeoplemon& victim, int dmg);
    void applyAilmentFromMove(Battler& owner, pplmn::BattlePeoplemon& victim,
                              pplmn::Ailment ailment);
    void applyAilmentFromMove(Battler& owner, pplmn::BattlePeoplemon& victim,
                              pplmn::PassiveAilment ailment);
    void doStatChange(pplmn::BattlePeoplemon& recv, pplmn::Stat stat, int amt,
                      bool playAnim = true);
    void doRoar(Battler& victim);

    void handleBattlerRoundEnd(Battler& battler);
};

} // namespace battle
} // namespace core

#endif
