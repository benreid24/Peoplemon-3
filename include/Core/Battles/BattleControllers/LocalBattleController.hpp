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
    Battler* currentFainter;
    Battler* midturnSwitcher;
    unsigned int xpAward;
    unsigned int xpAwardRemaining;
    int xpAwardIndex;
    pplmn::MoveId learnMove;
    bool firstTurn;

    // for attack phases
    pplmn::MoveId usedMove;
    pplmn::MoveEffect effect;
    int damage;

    virtual void onCommandQueued(const Command& cmd) override;
    virtual void onCommandProcessed(const Command& cmd) override;
    virtual void onUpdate(bool viewSynced, bool queueEmpty) override;

    void setBattleState(BattleState::Stage nextState);
    void initCurrentStage();
    void checkCurrentStage(bool viewSynced, bool queueEmpty);
    BattleState::Stage getNextStage(BattleState::Stage ns);
    int getPriority(Battler& battler);

    void startUseMove(Battler& user, int index);
    void checkAbilitiesAfterMove(Battler& user);
    void handleMoveEffect(Battler& user);
    float getEffectivenessMultiplier(pplmn::BattlePeoplemon& attacker,
                                     pplmn::BattlePeoplemon& defender, pplmn::MoveId move,
                                     pplmn::Type moveType);
    bool checkMoveCancelled(Battler& user, Battler& victim, int i, pplmn::MoveId move, int pwr,
                            pplmn::Type moveType, pplmn::MoveEffect effect,
                            bool isChargeSecondTurn);
    void applyDamageWithChecks(Battler& owner, pplmn::BattlePeoplemon& victim, pplmn::MoveId move,
                               int dmg);
    void applyAilmentFromMove(Battler& owner, pplmn::BattlePeoplemon& victim,
                              pplmn::Ailment ailment);
    void applyAilmentFromMove(Battler& owner, pplmn::BattlePeoplemon& victim,
                              pplmn::PassiveAilment ailment);
    void doStatChange(pplmn::BattlePeoplemon& recv, pplmn::Stat stat, int amt,
                      bool playAnim = true);
    void doRoar(Battler& victim);

    void handleBattlerTurnStart(Battler& battler);
    void handleBattlerRoundEnd(Battler& battler);

    bool canSwitch(Battler& battler);
    bool tryMidturnSwitch(Battler& battler);
    void startSwitch(Battler& battler);
    void doSwitch(Battler& battler, unsigned int newPP);
    void postSwitch(Battler& battler);

    bool isFainter(Battler& battler) const;
    void preFaint(Battler& fainter);

    void checkKlutz(Battler& battler);

    static bool teachThisTurn(Battler& battler);
};

} // namespace battle
} // namespace core

#endif
