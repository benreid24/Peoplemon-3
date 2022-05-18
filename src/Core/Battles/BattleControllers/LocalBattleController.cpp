#include <Core/Battles/BattleControllers/LocalBattleController.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Battles/Battle.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Battles/BattleView.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Move.hpp>

namespace core
{
namespace battle
{
namespace
{
int critChance(int stage) {
    switch (stage) {
    case 0:
        return 7;
    case 1:
        return 13;
    case 2:
        return 25;
    case 3:
        return 33;
    default:
        return 50;
    }
}

bool isPeanutAllergic(pplmn::Id ppl) {
    using Id = pplmn::Id;
    switch (ppl) {
    case Id::EmanualA:
    case Id::EmanualB:
    case Id::EmanualC:
    case Id::QuinnA:
    case Id::QuinnB:
    case Id::QuinnC:
    case Id::MattA:
    case Id::MattB:
    case Id::MattC:
    case Id::JudasA:
    case Id::JudasB:
        return true;
    default:
        return false;
    }
}

bool doEvenIfDead(pplmn::MoveEffect effect) {
    using E = pplmn::MoveEffect;

    switch (effect) {
    case E::WakeBoth: // TODO - any others?
        return true;
    default:
        return false;
    }
}

int computeDamage(int pwr, int atk, int def, int userLevel) {
    const int base = ((2 * userLevel / 5 + 2) * (atk / def) * pwr) / 50 + 2;
    const float m  = bl::util::Random::get<float>(0.85f, 1.f);
    return static_cast<int>(static_cast<float>(base) * m);
}

bool isTeachMove(pplmn::MoveId move) {
    switch (move) {
    case pplmn::MoveId::Teach:
    case pplmn::MoveId::MegaTeach:
    case pplmn::MoveId::SuperTeach:
    case pplmn::MoveId::ExtremeTeach:
        return true;
    default:
        return false;
    }
}

bool isJokeMove(pplmn::MoveId move) {
    switch (move) {
    case pplmn::MoveId::BadJoke:
    case pplmn::MoveId::SubtleJoke:
        return true;
    default:
        return false;
    }
}

class BattlerAttackFinalizer {
public:
    BattlerAttackFinalizer(Battler& b, pplmn::MoveId move)
    : battler(b)
    , move(move) {}

    ~BattlerAttackFinalizer() { battler.getSubstate().lastMoveUsed = move; }

private:
    Battler& battler;
    const pplmn::MoveId move;
};

} // namespace

LocalBattleController::LocalBattleController()
: currentStageInitialized(false)
, finalEffectsApplied(false)
, currentFainter(nullptr)
, midturnSwitcher(nullptr)
, xpAward(0)
, xpAwardRemaining(0)
, xpAwardIndex(-1)
, learnMove(pplmn::MoveId::Unknown)
, firstTurn(true) {}

void LocalBattleController::updateBattleState(bool viewSynced, bool queueEmpty) {
    if (!currentStageInitialized) {
        if (viewSynced && queueEmpty) {
            initCurrentStage();
            currentStageInitialized = true;
        }
    }
    else {
        checkCurrentStage(viewSynced, queueEmpty);
    }
}

void LocalBattleController::initCurrentStage() {
    using Stage = BattleState::Stage;

    switch (state->currentStage()) {
    case Stage::WildIntro:
        // TODO - play anim?
        queueCommand({Command::SyncStateSwitch, &state->enemy() == &state->activeBattler()});
        queueCommand({cmd::Message(cmd::Message::Type::WildIntro)}, true);
        break;

    case Stage::TrainerIntro:
        // TODO - show image somehow
        queueCommand({cmd::Message(cmd::Message::Type::TrainerIntro)}, true);
        break;

    case Stage::NetworkIntro:
        // TODO - show image?
        queueCommand({cmd::Message(cmd::Message::Type::NetworkIntro)}, true);
        break;

    case Stage::IntroSendInSelf:
        queueCommand({Command::SyncStateSwitch, &state->localPlayer() == &state->activeBattler()});
        queueCommand({cmd::Message(cmd::Message::Type::PlayerFirstSendout)});
        queueCommand({cmd::Animation(cmd::Animation::Type::PlayerFirstSendout)}, true);
        checkKlutz(state->localPlayer());
        break;

    case Stage::IntroSendInOpponent:
        queueCommand({Command::SyncStateSwitch, &state->enemy() == &state->activeBattler()});
        queueCommand({cmd::Message(cmd::Message::Type::OpponentFirstSendout)});
        queueCommand({cmd::Animation(cmd::Animation::Type::OpponentFirstSendout)}, true);
        checkKlutz(state->enemy());
        break;

    case Stage::WaitingChoices:
        finalEffectsApplied = false;
        state->localPlayer().pickAction();
        state->enemy().pickAction();
        break;

    case Stage::TurnStart:
        handleBattlerTurnStart(state->activeBattler());
        break;

    case Stage::PreUseItem:
        queueCommand({cmd::Message(cmd::Message::Type::PreUseItem,
                                   state->activeBattler().chosenItem(),
                                   true)},
                     true);
        break;

    case Stage::UsingItem:
        if (item::Item::hasEffectOnPeoplemon(
                state->activeBattler().chosenItem(),
                state->activeBattler().peoplemon()[state->activeBattler().chosenPeoplemon()],
                state->activeBattler())) {
            queueCommand({cmd::Message(cmd::Message::Type::ItemUseResult,
                                       state->activeBattler().chosenPeoplemon(),
                                       state->activeBattler().chosenItem())});
            item::Item::useOnPeoplemon(
                state->activeBattler().chosenItem(),
                state->activeBattler().peoplemon()[state->activeBattler().chosenPeoplemon()],
                state->activeBattler());
            state->activeBattler().removeItem(state->activeBattler().chosenItem());
            queueCommand({Command::SyncStateNoSwitch}, true);
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::ItemNoEffect)}, true);
        }
        break;

    case Stage::BeforeSwitch:
        if (canSwitch(state->activeBattler())) { startSwitch(state->activeBattler()); }
        else {
            setBattleState(Stage::NextBattler);
        }
        break;

    case Stage::Switching:
        doSwitch(state->activeBattler(), state->activeBattler().chosenPeoplemon());
        break;

    case Stage::AfterSwitch:
        postSwitch(state->activeBattler());
        break;

    case Stage::BeforeRun:
        // TODO - show message and determine result
        // TODO - take RunAway ability into account
        break;

    case Stage::Running:
        // TODO - display message
        break;

    case Stage::RunFailed:
        // TODO - display message
        break;

    case Stage::Attacking:
        startUseMove(state->activeBattler(), state->activeBattler().chosenMove());
        break;

    case Stage::ResolveAfterAttackAbilities:
        checkAbilitiesAfterMove(state->activeBattler());
        break;

    case Stage::ResolveAttackEffect:
        handleMoveEffect(state->activeBattler());
        break;

    case Stage::WaitingMidTurnSwitch:
        midturnSwitcher->pickPeoplemon(false, false);
        break;

    case Stage::BeforeMidTurnSwitch:
        startSwitch(*midturnSwitcher);
        break;

    case Stage::MidTurnSwitching:
        doSwitch(*midturnSwitcher, midturnSwitcher->chosenPeoplemon());
        break;

    case Stage::AfterMidTurnSwitch:
        postSwitch(*midturnSwitcher);
        break;

    case Stage::RoarSwitching:
        doSwitch(*midturnSwitcher, midturnSwitcher->selectRandomPeoplemon());
        break;

    case Stage::AfterRoarSwitch:
        postSwitch(*midturnSwitcher);
        break;

    case Stage::Fainting:
        if (isFainter(state->inactiveBattler())) { currentFainter = &state->inactiveBattler(); }
        else if (isFainter(state->activeBattler())) {
            currentFainter = &state->activeBattler();
        }
        else {
            BL_LOG_CRITICAL << "In faint state but neither battler has fainted";
            setBattleState(Stage::Completed);
        }
        if (currentFainter != nullptr) {
            currentFainter->getSubstate().faintHandled = true;
            preFaint(*currentFainter);
        }
        break;

    case Stage::XpAwardPeoplemonBegin:
        state->localPlayer().peoplemon()[xpAwardIndex].base().awardEVs(
            pplmn::Peoplemon::evAward(currentFainter->activePeoplemon().base().id()));
        queueCommand({cmd::Message(cmd::Message::Type::AwardedXp, xpAwardIndex, xpAwardRemaining)},
                     true);
        break;

    case Stage::XpAwarding:
        xpAwardRemaining =
            state->localPlayer().peoplemon()[xpAwardIndex].base().awardXP(xpAwardRemaining);
        queueCommand({Command::SyncStateNoSwitch}, true);
        break;

    case Stage::LevelingUp:
        // TODO - determine best way to defer evolution to after battle
        learnMove = state->localPlayer().peoplemon()[xpAwardIndex].base().levelUp();
        queueCommand({cmd::Message(cmd::Message::Type::LevelUp, xpAwardIndex)}, true);
        queueCommand({Command::SyncStateNoSwitch}, true);
        // TODO - do we want to show stat increase box?
        break;

    case Stage::WaitingLearnMoveChoice:
        queueCommand({cmd::Message(cmd::Message::Type::AskForgetMove, learnMove)}, true);
        break;

    case Stage::WaitingForgetMoveChoice:
        battle->view.menu().chooseMoveToForget();
        break;

    case Stage::WaitingFaintSwitch:
        currentFainter->pickPeoplemon(true, currentFainter->getSubstate().koReviveHp > 0);
        break;

    case Stage::FaintSwitching:
        doSwitch(*currentFainter, currentFainter->chosenPeoplemon());
        break;

    case Stage::AfterFaintSwitch:
        postSwitch(*currentFainter);
        currentFainter = nullptr;
        break;

    case Stage::RoundFinalEffectsPlayer:
        handleBattlerRoundEnd(state->localPlayer());
        break;

    case Stage::RoundFinalEffectsEnemy:
        handleBattlerRoundEnd(state->enemy());
        break;

    case Stage::TrainerDefeated:
        battle->localPlayerWon = true;
        battle->player.state().monei += state->enemy().prizeMoney();
        queueCommand({cmd::Message(cmd::Message::Type::TrainerLost)}, true);
        queueCommand(
            {cmd::Message(cmd::Message::Type::WonMoney, state->enemy().prizeMoney(), false)}, true);
        break;

    case Stage::PeoplemonCaught:
        battle->localPlayerWon = true;
        // TODO - display message, give peoplemon
        // TODO - maybe show peopledex
        break;

    case Stage::NetworkDefeated:
        battle->localPlayerWon = true;
        [[fallthrough]];

    case Stage::NetworkLost:
        queueCommand({cmd::Message(cmd::Message::Type::NetworkWinLose)}, true);
        break;

    case Stage::Whiteout:
        queueCommand({cmd::Message(cmd::Message::Type::WhiteoutA)}, true);
        queueCommand({cmd::Message(cmd::Message::Type::WhiteoutB)}, true);
        break;

    case Stage::NetworkDisconnect:
    case Stage::NextBattler:
    case Stage::Completed:
    case Stage::RoundStart:
    case Stage::CheckPlayerContinue:
    case Stage::WaitingPlayerContinue:
    case Stage::CheckFaint:
    case Stage::XpAwardBegin:
    case Stage::RoundEnd:
        // do nothing, these are intermediate states
        break;

    default:
        BL_LOG_CRITICAL << "Invalid battle stage: " << state->currentStage();
        setBattleState(Stage::Completed);
        break;
    }
}

void LocalBattleController::checkCurrentStage(bool viewSynced, bool queueEmpty) {
    using Stage = BattleState::Stage;

    if (viewSynced && queueEmpty) {
        switch (state->currentStage()) {
        case Stage::WildIntro:
            setBattleState(Stage::IntroSendInSelf);
            break;

        case Stage::TrainerIntro:
            setBattleState(Stage::IntroSendInOpponent);
            break;

        case Stage::NetworkIntro:
            setBattleState(Stage::IntroSendInOpponent);
            break;

        case Stage::IntroSendInSelf:
            setBattleState(Stage::WaitingChoices);
            break;

        case Stage::IntroSendInOpponent:
            setBattleState(Stage::IntroSendInSelf);
            break;

        case Stage::WaitingChoices:
            if (state->localPlayer().actionSelected() && state->enemy().actionSelected()) {
                setBattleState(Stage::RoundStart);
            }
            break;

        case Stage::TurnStart:
            switch (state->activeBattler().chosenAction()) {
            case TurnAction::Fight:
                setBattleState(Stage::Attacking);
                break;
            case TurnAction::Item:
                setBattleState(Stage::PreUseItem);
                break;
            case TurnAction::Run:
                setBattleState(Stage::BeforeRun);
                break;
            case TurnAction::Switch:
                setBattleState(Stage::BeforeSwitch);
                break;
            default:
                BL_LOG_ERROR << "Unknown turn action: " << state->activeBattler().chosenAction();
                setBattleState(Stage::Completed);
                break;
            }
            break;

        case Stage::PreUseItem:
            setBattleState(Stage::UsingItem);
            break;

        case Stage::UsingItem:
            setBattleState(Stage::NextBattler);
            break;

        case Stage::BeforeSwitch:
            setBattleState(Stage::Switching);
            break;

        case Stage::Switching:
            setBattleState(Stage::AfterSwitch);
            break;

        case Stage::AfterSwitch:
            setBattleState(Stage::NextBattler);
            break;

        case Stage::BeforeRun:
            // TODO - how to carry over the result of the run?
            setBattleState(Stage::Running);
            break;

        case Stage::Running:
            setBattleState(Stage::Completed);
            break;

        case Stage::RunFailed:
            setBattleState(Stage::NextBattler);
            break;

        case Stage::Attacking:
            setBattleState(Stage::ResolveAfterAttackAbilities);
            break;

        case Stage::ResolveAfterAttackAbilities:
            queueCommand({Command::SyncStateNoSwitch}, true);
            setBattleState(Stage::ResolveAttackEffect);
            break;

        case Stage::ResolveAttackEffect:
            queueCommand({Command::SyncStateNoSwitch}, true);
            setBattleState(Stage::NextBattler);
            break;

        case Stage::WaitingMidTurnSwitch:
            if (state->activeBattler().actionSelected()) {
                setBattleState(Stage::BeforeMidTurnSwitch);
            }
            break;

        case Stage::BeforeMidTurnSwitch:
            setBattleState(Stage::MidTurnSwitching);
            break;

        case Stage::MidTurnSwitching:
            setBattleState(Stage::AfterMidTurnSwitch);
            break;

        case Stage::AfterMidTurnSwitch:
            setBattleState(Stage::NextBattler);
            break;

        case Stage::RoarSwitching:
            setBattleState(Stage::AfterRoarSwitch);
            break;

        case Stage::AfterRoarSwitch:
            setBattleState(Stage::NextBattler);
            break;

        case Stage::RoundFinalEffectsPlayer:
            setBattleState(Stage::RoundFinalEffectsEnemy);
            break;

        case Stage::RoundFinalEffectsEnemy:
            setBattleState(Stage::RoundEnd);
            break;

        case Stage::Fainting:
            if (currentFainter != &state->localPlayer() && battle->type != Battle::Type::Online &&
                state->localPlayer().activePeoplemon().base().currentHp() > 0) {
                setBattleState(Stage::XpAwardBegin);
            }
            else {
                setBattleState(Stage::CheckPlayerContinue);
            }
            break;

        case Stage::XpAwardPeoplemonBegin:
            setBattleState(Stage::XpAwarding);
            break;

        case Stage::XpAwarding:
            if (xpAwardRemaining > 0) { setBattleState(Stage::LevelingUp); }
            else {
                xpAwardIndex = state->localPlayer().getNextXpEarnerIndex(xpAwardIndex);
                if (xpAwardIndex >= 0) {
                    xpAwardRemaining = xpAward;
                    if (state->localPlayer().peoplemon()[xpAwardIndex].base().hasExpShare()) {
                        xpAwardRemaining *= 2;
                    }
                    setBattleState(Stage::XpAwardPeoplemonBegin);
                }
                else {
                    setBattleState(Stage::CheckFaint);
                }
            }
            break;

        case Stage::LevelingUp:
            if (learnMove != pplmn::MoveId::Unknown) {
                if (state->localPlayer().peoplemon()[xpAwardIndex].base().gainMove(learnMove)) {
                    queueCommand(
                        {cmd::Message(cmd::Message::Type::LearnedMove, xpAwardIndex, learnMove)},
                        true);
                    setBattleState(Stage::XpAwarding);
                }
                else {
                    queueCommand({cmd::Message(cmd::Message::Type::TryingToLearnMove,
                                               xpAwardIndex,
                                               learnMove)},
                                 true);
                    setBattleState(Stage::WaitingLearnMoveChoice);
                }
            }
            else {
                setBattleState(Stage::XpAwarding);
            }
            break;

        case Stage::WaitingLearnMoveChoice:
            if (battle->view.playerChoseForgetMove()) {
                setBattleState(Stage::WaitingForgetMoveChoice);
            }
            else {
                queueCommand(
                    {cmd::Message(cmd::Message::Type::DidntLearnMove, xpAwardIndex, learnMove)},
                    true);
                setBattleState(Stage::XpAwarding);
            }
            break;

        case Stage::WaitingForgetMoveChoice:
            if (battle->view.menu().selectedMove() >= 0) {
                pplmn::BattlePeoplemon& ppl = state->localPlayer().peoplemon()[xpAwardIndex];
                pplmn::OwnedMove& m = ppl.base().knownMoves()[battle->view.menu().selectedMove()];
                queueCommand({cmd::Message(cmd::Message::Type::ForgotMove, xpAwardIndex, m.id)},
                             true);
                ppl.base().learnMove(learnMove, battle->view.menu().selectedMove());
                queueCommand(
                    {cmd::Message(cmd::Message::Type::LearnedMove, xpAwardIndex, learnMove)},
                    false);
            }
            else {
                queueCommand(
                    {cmd::Message(cmd::Message::Type::DidntLearnMove, xpAwardIndex, learnMove)},
                    true);
            }
            setBattleState(Stage::XpAwarding);
            break;

        case Stage::WaitingPlayerContinue:
            if (state->localPlayer().actionSelected()) {
                if (state->localPlayer().shouldContinue()) { setBattleState(Stage::CheckFaint); }
                else {
                    setBattleState(Stage::Completed);
                }
            }
            break;

        case Stage::WaitingFaintSwitch:
            if (currentFainter->actionSelected()) { setBattleState(Stage::FaintSwitching); }
            break;

        case Stage::FaintSwitching:
            setBattleState(Stage::AfterFaintSwitch);
            break;

        case Stage::AfterFaintSwitch:
            setBattleState(Stage::RoundEnd);
            break;

        case Stage::TrainerDefeated:
        case Stage::NetworkDefeated:
        case Stage::NetworkLost:
        case Stage::Whiteout:
        case Stage::PeoplemonCaught:
            setBattleState(Stage::Completed);
            break;

        case Stage::NetworkDisconnect:
        case Stage::NextBattler:
        case Stage::Completed:
        case Stage::RoundStart:
        case Stage::CheckPlayerContinue:
        case Stage::CheckFaint:
        case Stage::XpAwardBegin:
        case Stage::RoundEnd:
            // do nothing, these are intermediate states
            break;

        default:
            BL_LOG_CRITICAL << "Invalid battle stage: " << state->currentStage();
            setBattleState(Stage::Completed);
            break;
        }
    }
}

void LocalBattleController::setBattleState(BattleState::Stage ns) {
    using Stage = BattleState::Stage;

    Stage nns = getNextStage(ns);
    while (nns != ns) {
        ns  = nns;
        nns = getNextStage(nns);
    }

    state->setStage(ns);
    currentStageInitialized = false;
}

BattleState::Stage LocalBattleController::getNextStage(BattleState::Stage ns) {
    using Stage = BattleState::Stage;

    switch (ns) {
    case Stage::NextBattler:
        if (state->inactiveBattler().activePeoplemon().base().currentHp() == 0 ||
            state->activeBattler().activePeoplemon().base().currentHp() == 0) {
            return Stage::Fainting;
        }
        else {
            const BattleState::Stage next = state->nextTurn();
            queueCommand({Command::SyncStateNoSwitch});
            return next;
        }

    case Stage::RoundStart: {
        queueCommand({Command::SyncStateNoSwitch});
        int pp    = getPriority(state->localPlayer());
        int op    = getPriority(state->enemy());
        firstTurn = false;

        bool pfirst = true;
        if (pp < op) { pfirst = false; }
        else if (pp == op && state->localPlayer().chosenAction() == TurnAction::Fight) {
            const int lps = getSpeed(state->localPlayer());
            const int eps = getSpeed(state->enemy());
            if (eps > lps) { pfirst = false; }
            else if (lps == eps) {
                pfirst = bl::util::Random::get<int>(0, 100) <= 50;
            }
        }

        // TODO - active battler may change here, invalidating messages
        state->beginRound(pfirst);
        queueCommand({Command::SyncStateNoSwitch});
        state->localPlayer().activePeoplemon().notifyInBattle();
        state->enemy().activePeoplemon().notifyInBattle();
        return Stage::TurnStart;
    }

    case Stage::RoundEnd:
        if (state->inactiveBattler().activePeoplemon().base().currentHp() == 0 ||
            state->activeBattler().activePeoplemon().base().currentHp() == 0) {
            return Stage::Fainting;
        }
        else {
            queueCommand({Command::SyncStateNoSwitch});
            return Stage::WaitingChoices;
        }

    case Stage::CheckPlayerContinue:
        if (currentFainter == &state->localPlayer() &&
            battle->type == Battle::Type::WildPeoplemon) {
            state->localPlayer().startChooseToContinue();
            return Stage::WaitingPlayerContinue;
        }
        return Stage::CheckFaint;

    case Stage::CheckFaint:
        // TODO - maybe determine if the other battler wants to switch out
        if (currentFainter->canFight()) { return Stage::WaitingFaintSwitch; }
        else {
            const bool playerActive = &state->localPlayer() == &state->activeBattler();
            if (!state->enemy().canFight() && state->localPlayer().canFight()) { // player won
                queueCommand({Command::NotifyBattleWinner, playerActive});
                switch (battle->type) {
                case Battle::Type::Trainer:
                    return Stage::TrainerDefeated;
                case Battle::Type::Online:
                    return Stage::NetworkDefeated;
                case Battle::Type::WildPeoplemon:
                default:
                    return Stage::Completed;
                }
            }
            else { // player lost
                queueCommand({Command::NotifyBattleWinner, !playerActive});
                return Stage::Whiteout;
            }
        }
        break;

    case Stage::XpAwardBegin:
        xpAward =
            currentFainter->activePeoplemon().base().xpYield(battle->type == Battle::Type::Trainer);
        xpAward /= state->localPlayer().xpEarnerCount();
        xpAwardRemaining = xpAward;
        xpAwardIndex     = state->localPlayer().getFirstXpEarner();
        return xpAwardIndex >= 0 ? Stage::XpAwardPeoplemonBegin : Stage::CheckFaint;

    default:
        return ns;
    }
}

void LocalBattleController::onCommandQueued(const Command&) {}

void LocalBattleController::onCommandProcessed(const Command&) {}

void LocalBattleController::onUpdate(bool vs, bool qe) { updateBattleState(vs, qe); }

void LocalBattleController::startUseMove(Battler& user, int index) {
    static pplmn::OwnedMove skimpOut(pplmn::MoveId::SkimpOut);

    Battler& victim = &user == &state->localPlayer() ? state->enemy() : state->localPlayer();
    pplmn::BattlePeoplemon& attacker = user.activePeoplemon();
    pplmn::BattlePeoplemon& defender = victim.activePeoplemon();
    const bool userIsActive          = &user == &state->activeBattler();
    user.getSubstate().lastMoveIndex = index;

    // check if frozen
    if (attacker.hasAilment(pplmn::Ailment::Frozen)) {
        if (bl::util::Random::get<int>(0, 100) <= 20) {
            attacker.clearAilments(nullptr);
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::ThawedOut, userIsActive)}, true);
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::FrozenAilment, userIsActive)}, true);
            return;
        }
    }

    // check if enough pp
    if (!user.getSubstate().gotBaked) {
        if (index < 0 || user.activePeoplemon().base().knownMoves()[index].curPP == 0) {
            user.activePeoplemon().base().currentHp() = 0;
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::NoPPDeath, userIsActive)}, true);
            return;
        }
    }

    // get move, handle charge, and deduct pp
    pplmn::OwnedMove& move =
        !user.getSubstate().gotBaked ? user.activePeoplemon().base().knownMoves()[index] : skimpOut;
    const bool isChargeSecondTurn = user.getSubstate().chargingMove >= 0;
    if (!isChargeSecondTurn) { move.curPP -= 1; }
    else {
        user.getSubstate().chargingMove = -1;
    }

    // determine actual move to use based on effects
    usedMove                          = move.id;
    const pplmn::MoveEffect preEffect = pplmn::Move::effect(move.id);
    if (preEffect == pplmn::MoveEffect::RandomMove) {
        usedMove = pplmn::Move::getRandomMove(false);
    }

    BattlerAttackFinalizer _finalizer(user, usedMove);

    // determine if hit
    int acc = pplmn::Move::accuracy(usedMove);
    if (attacker.base().holdItem() == item::Id::Glasses) {
        acc = acc * 11 / 10;
        queueCommand({cmd::Message(cmd::Message::Type::GlassesAcc, userIsActive)}, true);
    }
    const int accStage  = attacker.battleStages().acc;
    const int evdStage  = defender.battleStages().evade;
    const int stage     = std::max(std::min(accStage - evdStage, 6), -6);
    const int hitChance = acc * pplmn::BattleStats::getAccuracyMultiplier(stage);
    bool hit            = bl::util::Random::get<int>(0, 100) <= hitChance || acc == 0;
    effect              = pplmn::Move::effect(usedMove);

    const auto printAttackPrequel = [this, &move, isChargeSecondTurn, userIsActive]() {
        // print used move
        if (!isChargeSecondTurn) { queueCommand({cmd::Message(cmd::Message(move.id))}, true); }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::ChargeUnleashed, userIsActive)}, true);
        }

        // random move
        if (move.id != usedMove) {
            queueCommand({cmd::Message(cmd::Message::Type::RandomMove, move.id, usedMove)}, true);
        }
    };

    // volleyball moves guaranteed to hit if ball is set
    if (user.getSubstate().ballSet &&
        (effect == pplmn::MoveEffect::SwipeBall || effect == pplmn::MoveEffect::SpikeBall)) {
        hit = true;
    }

    // bail out if we missed
    if (!hit) {
        if (attacker.currentAbility() == pplmn::SpecialAbility::FakeStudy) {
            queueCommand({cmd::Message(cmd::Message::Type::FakeStudyAbility, userIsActive)}, true);
        }
        else {
            printAttackPrequel();
            queueCommand({cmd::Message::Type::AttackMissed}, true);
        }
        setBattleState(BattleState::Stage::NextBattler);
        return;
    }
    printAttackPrequel();

    // mark this move as hitting
    if (usedMove == pplmn::MoveId::Kick) { victim.getSubstate().move64Hit = true; }

    // determine if attacking move
    const pplmn::Type moveType = pplmn::Move::type(usedMove);
    const bool special         = pplmn::Move::isSpecial(usedMove);
    int pwr                    = pplmn::Move::damage(usedMove);

    // check if abilities or substate canceles the move
    if (checkMoveCancelled(
            user, victim, index, usedMove, pwr, moveType, effect, isChargeSecondTurn)) {
        return;
    }

    // check if move has Gamble
    if (effect == pplmn::MoveEffect::Gamble) {
        const std::int16_t r = bl::util::Random::get<std::int16_t>(1, 20);
        if (r == 1) {
            user.activePeoplemon().base().currentHp() = 1;
            queueCommand({cmd::Message(cmd::Message::Type::GambleOne, userIsActive)}, true);
        }
        else if (r == 20) {
            pwr = 200;
            queueCommand({cmd::Message(cmd::Message::Type::GambleTwenty, userIsActive)}, true);
        }
        else {
            pwr = r * 5;
            queueCommand({cmd::Message(cmd::Message::Type::GambleMiddle, r, userIsActive)}, true);
        }
    }

    // check if move has DoubleFamily
    if (effect == pplmn::MoveEffect::DoubleFamily) {
        const int bc = user.getBroCount();
        pwr          = static_cast<float>(pwr) * (static_cast<float>(bc) * 1.5f);
        queueCommand({cmd::Message(cmd::Message::Type::DoubleBroPower, bc, userIsActive)}, true);
    }

    // check if user has BeefedUp or Reserved abilities
    if (attacker.base().currentHp() <= attacker.currentStats().hp / 4 && pwr > 0) {
        if (attacker.currentAbility() == pplmn::SpecialAbility::BeefedUp &&
            moveType == pplmn::Type::Athletic) {
            pwr = pwr * 5 / 4;
            queueCommand({cmd::Message(cmd::Message::Type::BeefyPower, userIsActive)}, true);
        }
        if (attacker.currentAbility() == pplmn::SpecialAbility::Reserved &&
            moveType == pplmn::Type::Quiet) {
            pwr = pwr * 5 / 4;
            queueCommand({cmd::Message(cmd::Message::Type::ReservedQuietPower, userIsActive)},
                         true);
        }
    }

    // Check DukeOfJokes ability
    if (attacker.currentAbility() == pplmn::SpecialAbility::DukeOfJokes && isJokeMove(usedMove)) {
        pwr = pwr * 3 / 2;
        queueCommand({cmd::Message(cmd::Message::Type::DukeOfJokes, userIsActive)}, true);
    }

    // begin determining how much damage to deal
    damage = 0;

    // check counter and mirror coat
    if (effect == pplmn::MoveEffect::Counter) {
        if (!pplmn::Move::isSpecial(user.getSubstate().lastMoveHitWith) &&
            user.getSubstate().lastDamageTaken > 0) {
            damage = user.getSubstate().lastDamageTaken * 2;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::GenericMoveFailed, userIsActive)}, true);
            return;
        }
    }
    if (effect == pplmn::MoveEffect::MirrorCoat) {
        if (pplmn::Move::isSpecial(user.getSubstate().lastMoveHitWith) &&
            user.getSubstate().lastDamageTaken > 0) {
            damage = user.getSubstate().lastDamageTaken * 2;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::GenericMoveFailed, userIsActive)}, true);
            return;
        }
    }

    // finally play the animation
    queueCommand({cmd::Animation(!userIsActive, usedMove)}, true);
    // TODO - play peoplemon animation for attacker?

    // move does damage
    if (pwr > 0) {
        queueCommand({cmd::Animation(!userIsActive, cmd::Animation::Type::ShakeAndFlash)});
        queueCommand({Command::SyncStateNoSwitch}, true);

        int atk          = special ? attacker.currentStats().spatk : attacker.currentStats().atk;
        int def          = special ? defender.currentStats().spdef : defender.currentStats().def;
        const float stab = pplmn::TypeUtil::getStab(moveType, attacker.base().type());
        float effective  = getEffectivenessMultiplier(attacker, defender, usedMove, moveType);
        bool crit = bl::util::Random::get<int>(0, 100) <= critChance(attacker.battleStages().crit);
        if (damage == 0) {
            // Check chillaxed ability
            if (crit && defender.currentAbility() == pplmn::SpecialAbility::Chillaxed) {
                crit = false;
                queueCommand({cmd::Message(cmd::Message::Type::ChillaxCritBlocked, userIsActive)},
                             true);
            }

            // check backwards hoody hold item
            if (attacker.base().holdItem() == item::Id::BackwardsHoody) {
                atk = atk * 3 / 2;
                queueCommand(
                    {cmd::Message(cmd::Message::Type::BackwardsHoodyStatDown, userIsActive)}, true);
            }

            // check spoon hold item
            if (attacker.base().holdItem() == item::Id::Spoon && special) {
                atk = atk * 11 / 10;
                queueCommand({cmd::Message(cmd::Message::Type::SpoonDamage, userIsActive)}, true);
            }

            // check slapping glove hold item
            if (attacker.base().holdItem() == item::Id::SlappingGlove && !special) {
                atk = atk * 11 / 10;
                queueCommand({cmd::Message(cmd::Message::Type::SlappingGloveDamage, userIsActive)},
                             true);
            }

            // check power juice
            if (attacker.base().currentHp() <= attacker.currentStats().hp / 4) {
                if (attacker.base().holdItem() == item::Id::PowerJuice) {
                    attacker.base().holdItem() = item::Id::None;
                    atk                        = atk * 11 / 10;
                    queueCommand({cmd::Message(cmd::Message::Type::PowerJuice, userIsActive)},
                                 true);
                }
            }

            // check iced tea hold item
            if (defender.base().currentHp() <= defender.currentStats().hp / 4) {
                if (defender.base().holdItem() == item::Id::IcedTea) {
                    defender.base().holdItem() = item::Id::None;
                    def                        = def * 11 / 10;
                    queueCommand({cmd::Message(cmd::Message::Type::IcedTea, !userIsActive)}, true);
                }
            }

            damage = computeDamage(pwr, atk, def, attacker.base().currentLevel());
            damage = static_cast<int>(static_cast<float>(damage) * stab * effective);
            if (crit) { damage *= 2; }
        }

        // reduce damage if Frustrated
        if (attacker.hasAilment(pplmn::Ailment::Frustrated) && !special) { damage /= 2; }

        BL_LOG_INFO << pplmn::Move::name(move.id) << " (pwr " << pwr << ") did " << damage
                    << " damage to " << defender.base().name();
        applyDamageWithChecks(victim, defender, usedMove, damage);

        if (crit) { queueCommand({cmd::Message(cmd::Message::Type::CriticalHit)}, true); }
        if (effective > 1.1f) {
            victim.getSubstate().lastMoveSuperEffective = usedMove;
            victim.getSubstate().preserveLastSuper      = true;
            queueCommand({cmd::Message(cmd::Message::Type::SuperEffective)}, true);
        }
        else if (effective < 0.9f) {
            queueCommand({cmd::Message(cmd::Message::Type::NotEffective)}, true);
        }

        queueCommand({Command::SyncStateNoSwitch}, true);
    }

    // ensure that everything is reflected and wait for the view
    queueCommand({Command::SyncStateNoSwitch}, true);
}

bool LocalBattleController::checkMoveCancelled(Battler& user, Battler& victim, int i,
                                               pplmn::MoveId move, int pwr, pplmn::Type moveType,
                                               pplmn::MoveEffect effect, bool isChargeSecondTurn) {
    const bool targetsVictim  = pwr > 0 || (!pplmn::Move::affectsUser(move) &&
                                           pplmn::Move::effect(move) != pplmn::MoveEffect::None);
    const bool userIsActive   = &user == &state->activeBattler();
    const bool victimIsActive = !userIsActive;

    // Check if sleeping
    if (user.activePeoplemon().hasAilment(pplmn::Ailment::Sleep)) {
        // waking up is handled at turn start
        queueCommand({cmd::Message(cmd::Message::Type::SleepingAilment, userIsActive)}, true);
        return true;
    }

    // Check if distracted
    if (user.getSubstate().hasAilment(pplmn::PassiveAilment::Distracted)) {
        queueCommand({cmd::Message(cmd::Message::Type::DistractedAilment, userIsActive)}, true);
        return true;
    }

    // Check if no joke teach ability
    if (victim.activePeoplemon().currentAbility() == pplmn::SpecialAbility::NoJokeTeach) {
        if (isJokeMove(move) && teachThisTurn(victim)) {
            queueCommand({cmd::Message(cmd::Message::Type::NoJokeTeachAbility, userIsActive)},
                         true);
            return true;
        }
    }

    // Check if confused
    if (user.getSubstate().hasAilment(pplmn::PassiveAilment::Confused)) {
        if (user.getSubstate().turnsConfused <= 0) {
            user.getSubstate().clearAilment(pplmn::PassiveAilment::Confused);
            queueCommand({cmd::Message(cmd::Message::Type::SnappedConfusion, userIsActive)}, true);
        }
        else {
            if (bl::util::Random::get<int>(0, 100) <= 50) {
                const int atk = user.activePeoplemon().currentStats().atk;
                const int def = user.activePeoplemon().currentStats().def;
                const int lvl = user.activePeoplemon().base().currentLevel();
                const int dmg = computeDamage(40, atk, def, lvl);
                user.activePeoplemon().applyDamage(dmg);
                queueCommand({Command::SyncStateNoSwitch});
                queueCommand({cmd::Message(cmd::Message::Type::HurtConfusion, userIsActive)}, true);
                return true;
            }
        }
    }

    // Check if annoyed
    if (user.activePeoplemon().hasAilment(pplmn::Ailment::Annoyed)) {
        if (bl::util::Random::get<int>(0, 100) <= 25) {
            queueCommand({cmd::Message(cmd::Message::Type::AnnoyAilment, userIsActive)}, true);
            return true;
        }
    }

    // check if move does not affect the victim
    if (targetsVictim) {
        const float effective =
            pplmn::TypeUtil::getSuperMult(moveType, victim.activePeoplemon().base().type());
        if (effective < 0.1f) {
            queueCommand({cmd::Message(cmd::Message::Type::IsNotAffected)}, true);
            return true;
        }
    }

    // check if victim is protected
    if (targetsVictim && victim.getSubstate().isProtected) {
        queueCommand({cmd::Message(cmd::Message::Type::WasProtected, victimIsActive)}, true);
        return true;
    }

    // stop here if this is first move of a charging move
    if (effect == pplmn::MoveEffect::Charge && !isChargeSecondTurn) {
        user.getSubstate().chargingMove = i;
        queueCommand({cmd::Message(cmd::Message::Type::ChargeStarted, userIsActive)}, true);
        return true;
    }

    // check OnlySleeping effect
    if (effect == pplmn::MoveEffect::OnlySleeping &&
        victim.activePeoplemon().base().currentAilment() != pplmn::Ailment::Sleep) {
        queueCommand({cmd::Message(cmd::Message::Type::OnlySleepAffected, userIsActive)}, true);
        return true;
    }

    // fail on 64
    if (effect == pplmn::MoveEffect::FailOnMove64 && user.getSubstate().move64Hit) {
        queueCommand({cmd::Message(cmd::Message::Type::Move64Cancel, move, userIsActive)}, true);
        return true;
    }

    // move goes ahead normally
    return false;
}

void LocalBattleController::applyDamageWithChecks(Battler& victim, pplmn::BattlePeoplemon& ppl,
                                                  pplmn::MoveId move, int dmg) {
    const bool isActive = &victim == &state->activeBattler();

    // check if victim has a Substitute
    if (victim.getSubstate().substituteHp > 0) {
        queueCommand({cmd::Message(cmd::Message::Type::SubstituteTookDamage, isActive)}, true);
        victim.getSubstate().substituteHp -= dmg;
        if (victim.getSubstate().substituteHp <= 0) {
            queueCommand({cmd::Message(cmd::Message::Type::SubstituteDied, isActive)}, true);
            victim.getSubstate().substituteHp = 0;
        }
        return; // no damage is dealt
    }

    // check Experienced Teacher ability
    if (teachThisTurn(victim) && ppl.base().currentHp() > 1 && dmg >= ppl.base().currentHp()) {
        dmg = ppl.base().currentHp() - 1;
        queueCommand({cmd::Message(cmd::Message::Type::ExperiencedTeachAbility, isActive)}, true);
    }

    // undying faith ability
    if (ppl.currentAbility() == pplmn::SpecialAbility::UndyingFaith &&
        dmg >= ppl.base().currentHp()) {
        if (bl::util::Random::get<int>(0, 100) <= 10) {
            dmg = ppl.base().currentHp() - 1;
            queueCommand({cmd::Message(cmd::Message::Type::UndyingFaithAbility, isActive)}, true);
        }
    }

    ppl.applyDamage(dmg);
    victim.getSubstate().lastMoveHitWith = move;
    victim.getSubstate().lastDamageTaken = dmg;

    // check if we Endure
    if (victim.getSubstate().enduringThisTurn && ppl.base().currentHp() == 0) {
        ppl.base().currentHp() = 1;
        queueCommand({cmd::Message(cmd::Message::Type::Endured, isActive)}, true);
    }
}

void LocalBattleController::applyAilmentFromMove(Battler& owner, pplmn::BattlePeoplemon& victim,
                                                 pplmn::Ailment ail) {
    const bool isActive = &victim == &state->activeBattler().activePeoplemon();

    // check too cool ability
    if (victim.currentAbility() == pplmn::SpecialAbility::TooCool &&
        ail == pplmn::Ailment::Annoyed) {
        queueCommand({cmd::Message(cmd::Message::Type::TooCoolAbility, isActive)}, true);
        return;
    }

    // all nighter ability
    if (victim.currentAbility() == pplmn::SpecialAbility::AllNighter &&
        ail == pplmn::Ailment::Sleep) {
        queueCommand({cmd::Message(cmd::Message::Type::AllNighterAbility, isActive)}, true);
        return;
    }

    // Substitute blocks ailments
    if (owner.getSubstate().substituteHp > 0) {
        queueCommand({cmd::Message(cmd::Message::Type::SubstituteAilmentBlocked, ail, isActive)});
        return;
    }

    // Guard blocks ailments
    if (owner.getSubstate().turnsGuarded > 0) {
        queueCommand({cmd::Message(cmd::Message::Type::GuardBlockedAilment, ail, isActive)});
        return;
    }

    // Classy blocks frustrated
    if (ail == pplmn::Ailment::Frustrated &&
        owner.activePeoplemon().currentAbility() == pplmn::SpecialAbility::Classy) {
        queueCommand({cmd::Message(cmd::Message::Type::ClassyFrustratedBlocked, isActive)}, true);
        return;
    }

    if (victim.giveAilment(ail)) {
        queueCommand({cmd::Message(cmd::Message::Type::GainedAilment, ail, isActive)});

        // Check SnackShare ailment give back ability
        if (victim.currentAbility() == pplmn::SpecialAbility::SnackShare) {
            Battler& other = isActive ? state->inactiveBattler() : state->activeBattler();
            if (other.activePeoplemon().giveAilment(ail)) {
                queueCommand(
                    {cmd::Message(cmd::Message::Type::GainedAilmentSnackshare, ail, !isActive)},
                    true);
            }
            else {
                // determine if blocked by too cool
                if (ail == pplmn::Ailment::Annoyed &&
                    other.activePeoplemon().currentAbility() == pplmn::SpecialAbility::TooCool) {
                    queueCommand({cmd::Message(cmd::Message::Type::TooCoolAbility, !isActive)},
                                 true);
                }
                else {
                    queueCommand(
                        {cmd::Message(cmd::Message::Type::AilmentGiveFail, ail, !isActive)}, true);
                }
            }
        }
    }
    else {
        queueCommand({cmd::Message(cmd::Message::Type::AilmentGiveFail, ail, isActive)}, true);
    }
}

void LocalBattleController::applyAilmentFromMove(Battler& owner, pplmn::BattlePeoplemon& victim,
                                                 pplmn::PassiveAilment ail) {
    const bool selfGive = &victim == &state->activeBattler().activePeoplemon();

    // prevent more than one ailment if ailment saturated
    if (victim.currentAbility() == pplmn::SpecialAbility::AilmentSaturated &&
        owner.getSubstate().ailments != pplmn::PassiveAilment::None) {
        queueCommand(
            {cmd::Message(cmd::Message::Type::AilmentSatPassiveAilmentBlocked, ail, selfGive)},
            true);
        return;
    }

    // Substitute blocks ailments
    if (owner.getSubstate().substituteHp > 0) {
        queueCommand(
            {cmd::Message(cmd::Message::Type::SubstitutePassiveAilmentBlocked, ail, selfGive)},
            true);
        return;
    }

    // Guard blocks ailments
    if (owner.getSubstate().turnsGuarded > 0) {
        queueCommand({cmd::Message(cmd::Message::Type::GuardBlockedPassiveAilment, ail, selfGive)},
                     true);
        return;
    }

    owner.getSubstate().giveAilment(ail);
    queueCommand({cmd::Message(cmd::Message::Type::GainedPassiveAilment, ail, selfGive)}, true);

    // TODO - reflect some passive ailments?
}

void LocalBattleController::doStatChange(pplmn::BattlePeoplemon& ppl, pplmn::Stat stat, int amt,
                                         bool anim) {
    const bool active = &ppl == &state->activeBattler().activePeoplemon();
    if (ppl.statChange(stat, amt)) {
        if (anim) {
            const auto at =
                amt > 0 ? cmd::Animation::Type::StatIncrease : cmd::Animation::Type::StatDecrease;
            queueCommand({cmd::Animation(active, at, stat)}, true);
        }
        if (amt > 1) {
            queueCommand({cmd::Message(cmd::Message::Type::StatIncreasedSharply, stat, active)},
                         true);
        }
        else if (amt > 0) {
            queueCommand({cmd::Message(cmd::Message::Type::StatIncreased, stat, active)}, true);
        }
        else if (amt > -2) {
            queueCommand({cmd::Message(cmd::Message::Type::StatDecreased, stat, active)}, true);
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::StatDecreasedSharply, stat, active)},
                         true);
        }
    }
    else {
        if (amt > 0) {
            queueCommand({cmd::Message(cmd::Message::Type::StatIncreaseFailed, stat, active)},
                         true);
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::StatDecreaseFailed, stat, active)},
                         true);
        }
    }
}

void LocalBattleController::handleBattlerTurnStart(Battler& battler) {
    const bool isActive         = &battler == &state->activeBattler();
    pplmn::BattlePeoplemon& ppl = battler.activePeoplemon();

    // alcoholic ability
    if (ppl.currentAbility() == pplmn::SpecialAbility::Alcoholic) {
        // TODO - check for alcohol in bag and consume
        queueCommand({cmd::Message(cmd::Message::Type::AlcoholicAbility, isActive)}, true);
    }

    // check if Sleep is done
    if (battler.activePeoplemon().turnsUntilAwake() <= 0) {
        if (ppl.base().currentAilment() == pplmn::Ailment::Sleep) {
            ppl.base().currentAilment() = pplmn::Ailment::None;
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::WokeUp, isActive)}, true);
        }
    }
}

void LocalBattleController::handleBattlerRoundEnd(Battler& battler) {
    const bool isActive         = &battler == &state->activeBattler();
    Battler& other              = isActive ? state->inactiveBattler() : state->activeBattler();
    pplmn::BattlePeoplemon& ppl = battler.activePeoplemon();
    battler.notifyTurnEnd();

    if (ppl.base().currentHp() == 0) {
        battler.getSubstate().ballBlocked = false;
        battler.getSubstate().ballSet     = false;
        battler.getSubstate().ballSpiked  = false;
        battler.getSubstate().ballSwiped  = false;
        return;
    }

    // check volleyball stuff
    // check if too many turns and we died
    const bool ballHandled = battler.getSubstate().ballSet || battler.getSubstate().ballBumped;
    if (battler.getSubstate().ballUpTurns >= 0 && !ballHandled) {
        battler.activePeoplemon().base().currentHp() = 0;
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::BallKillSelf, isActive)}, true);
        return;
    }
    // check if died from no switch
    if (battler.getSubstate().ballUpTurns >= 0 && battler.getSubstate().noOneToGetBall) {
        battler.activePeoplemon().base().currentHp() = 0;
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::BallNoSwitchSuicide, isActive)}, true);
        return;
    }
    // check if up too long
    if (battler.getSubstate().ballUpTurns >= 3) {
        battler.activePeoplemon().base().currentHp() = 0;
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::BallKillTimeout, isActive)}, true);
        return;
    }
    // check if killed by opponent
    if (other.getSubstate().ballUpTurns >= 0) {
        // check if killed by swipe
        if (other.getSubstate().ballSwiped && battler.getSubstate().ballBlocked) {
            battler.activePeoplemon().base().currentHp() = 0;
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::BallKillSwipe, isActive)}, true);
            return;
        }
        // check if killed by spike
        if (other.getSubstate().ballSpiked && !battler.getSubstate().ballBlocked) {
            battler.activePeoplemon().base().currentHp() = 0;
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::BallKillSpike, isActive)}, true);
            return;
        }
    }
    // check if killed by block
    if (battler.getSubstate().ballSpiked && other.getSubstate().ballBlocked) {
        battler.activePeoplemon().base().currentHp() = 0;
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::BallSpikeBlocked, isActive)}, true);
        return;
    }
    // check if killed by swipe no block
    if (battler.getSubstate().ballSwiped && !other.getSubstate().ballBlocked) {
        battler.activePeoplemon().base().currentHp() = 0;
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::BallSwipeBlocked, isActive)}, true);
        return;
    }
    battler.getSubstate().ballBlocked = false;
    battler.getSubstate().ballSet     = false;
    battler.getSubstate().ballSpiked  = false;
    battler.getSubstate().ballSwiped  = false;

    // check for HealNext
    if (battler.getSubstate().healNext >= 2) {
        ppl.giveHealth(ppl.currentStats().hp / 2);
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::HealNextHealed, isActive)}, true);
        battler.getSubstate().healNext = -1;
    }

    // check Stolen ailment
    if (battler.getSubstate().hasAilment(pplmn::PassiveAilment::Stolen)) {
        const int dmg = battler.activePeoplemon().currentStats().hp / 16;
        other.activePeoplemon().giveHealth(dmg);
        ppl.applyDamage(dmg);
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::StolenAilment, isActive)}, true);
        if (ppl.base().currentHp() == 0) { return; }
    }

    // check Frustrated ailment
    if (ppl.hasAilment(pplmn::Ailment::Frustrated)) {
        ppl.applyDamage(ppl.currentStats().hp / 16);
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::FrustratedAilment, isActive)}, true);
        if (ppl.base().currentHp() == 0) { return; }
    }

    // Check if sticky
    if (ppl.hasAilment(pplmn::Ailment::Sticky)) {
        ppl.applyDamage(ppl.currentStats().hp / 16 * battler.getSubstate().turnsSticky);
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::StickyAilment, isActive)}, true);
        if (ppl.base().currentHp() == 0) { return; }
    }

    // check for death counter (DieIn3Turns)
    if (battler.getSubstate().deathCounter == 0) {
        battler.activePeoplemon().base().currentHp() = 0;
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::DeathFromCountdown, isActive)}, true);
    }

    // check bag of goldfish hold item
    if (ppl.base().holdItem() == item::Id::BagOfGoldFish) {
        if (ppl.base().currentHp() < ppl.currentStats().hp) {
            ppl.giveHealth(ppl.currentStats().hp / 16);
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::BagOfGoldfish, isActive)}, true);
        }
    }

    // sketchy sack hold item
    if (ppl.base().holdItem() == item::Id::SketchySack &&
        ppl.base().currentHp() <= ppl.currentStats().hp / 2) {
        ppl.base().holdItem() = item::Id::None;
        ppl.applyDamage(ppl.currentStats().hp / 16);
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::SketchySack, isActive)}, true);
    }

    // check goldfish cracker hold item
    if (ppl.base().holdItem() == item::Id::GoldfishCracker &&
        ppl.base().currentHp() <= ppl.currentStats().hp / 2) {
        ppl.base().holdItem() = item::Id::None;
        ppl.giveHealth(ppl.currentStats().hp / 8);
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::GoldfishCracker, isActive)}, true);
    }

    // check wakeupbelle hold item
    if (ppl.base().holdItem() == item::Id::WakeUpBelle &&
        ppl.base().currentAilment() == pplmn::Ailment::Sleep) {
        ppl.base().currentAilment() = pplmn::Ailment::None;
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::WakeUpBelle, isActive)}, true);
    }

    // check super tiny mini fridge
    if (ppl.base().holdItem() == item::Id::SuperTinyMiniFridge && ppl.base().currentHp() > 0) {
        ppl.base().currentHp() = 0;
        queueCommand({cmd::Message(cmd::Message::Type::SuperTinyMiniFridge, isActive)}, true);
        // TODO - play explosion animation
        other.activePeoplemon().applyDamage(
            computeDamage(100,
                          ppl.currentStats().atk,
                          other.activePeoplemon().currentStats().def,
                          ppl.base().currentLevel()));
        queueCommand({Command::SyncStateNoSwitch}, true);
    }
}

void LocalBattleController::doRoar(Battler& victim) {
    const bool isActive = &victim == &state->activeBattler();

    if (!victim.canSwitch()) {
        queueCommand({cmd::Message(cmd::Message::Type::RoarFailedNoSwitch, isActive)}, true);
        return;
    }

    // adament prevents roar
    if (victim.activePeoplemon().currentAbility() == pplmn::SpecialAbility::Adament) {
        queueCommand({cmd::Message(cmd::Message::Type::AdamentAbility, isActive)}, true);
        return;
    }

    queueCommand({cmd::Message(cmd::Message::Type::Roar, isActive)});
    queueCommand({cmd::Animation(isActive, cmd::Animation::Type::SlideOut)}, true);
    setBattleState(BattleState::Stage::RoarSwitching);
    midturnSwitcher = &victim;
}

void LocalBattleController::startSwitch(Battler& battler) {
    const bool forActive = &battler == &state->activeBattler();
    queueCommand({cmd::Message(cmd::Message::Type::Callback, forActive)});
    queueCommand({cmd::Animation(forActive, cmd::Animation::Type::ComeBack)}, true);
}

void LocalBattleController::doSwitch(Battler& battler, unsigned int newPP) {
    const bool forActive           = &battler == &state->activeBattler();
    pplmn::BattlePeoplemon& oldPpl = battler.activePeoplemon();
    const std::int16_t oldIndex    = battler.outNowIndex();

    battler.setActivePeoplemon(newPP);
    queueCommand({cmd::Message(cmd::Message::Type::SendOut, forActive)}, true);
    queueCommand({Command::SyncStateSwitch, forActive});
    queueCommand({cmd::Animation(forActive, cmd::Animation::Type::SendOut)}, true);

    Battler& other = forActive ? state->inactiveBattler() : state->activeBattler();
    other.resetXpEarners();

    // check always friendly ability
    if (oldPpl.base().currentHp() > 0 &&
        oldPpl.currentAbility() == pplmn::SpecialAbility::AlwaysFriendly &&
        oldPpl.base().currentAilment() != pplmn::Ailment::None) {
        oldPpl.base().currentAilment() = pplmn::Ailment::None;
        queueCommand({cmd::Message(cmd::Message::Type::FriendlyAilmentHeal, oldIndex, forActive)},
                     true);
    }
}

void LocalBattleController::postSwitch(Battler& battler) {
    const bool forActive = &battler == &state->activeBattler();

    // check DeathSwap
    if (battler.getSubstate().koReviveHp > 0) {
        battler.activePeoplemon().giveHealth(battler.getSubstate().koReviveHp);
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::DeathSwapRevived,
                                   &battler == &state->activeBattler())},
                     true);
    }

    // Check baton pass stats
    if (battler.getSubstate().copyStatsFrom >= 0) {
        battler.activePeoplemon().copyStages(
            battler.peoplemon()[battler.getSubstate().copyStatsFrom]);
        battler.peoplemon()[battler.getSubstate().copyStatsFrom].resetStages();
        queueCommand({cmd::Message(cmd::Message::Type::BatonPassStatsCopied,
                                   battler.getSubstate().copyStatsFrom,
                                   forActive)},
                     true);
    }

    // check klutz ability
    checkKlutz(battler);

    // check total mom ability
    if (battler.getSubstate().totalMomIndex >= 0) {
        auto* moves = battler.activePeoplemon().base().knownMoves();
        for (int i = 0; i < 4; ++i) {
            if (moves[i].id != pplmn::MoveId::Unknown) { moves[i].restorePP(3); }
        }
        queueCommand({cmd::Message(cmd::Message::Type::TotalMomAbility,
                                   static_cast<std::int16_t>(battler.getSubstate().totalMomIndex),
                                   forActive)},
                     true);
    }

    battler.getSubstate().notifySwitch();
}

bool LocalBattleController::isFainter(Battler& b) const {
    if (b.activePeoplemon().base().currentHp() == 0) {
        return b.canFight() || !b.getSubstate().faintHandled;
    }
    return false;
}

void LocalBattleController::preFaint(Battler& b) {
    const bool isActive = &b == &state->activeBattler();

    queueCommand({cmd::Message(cmd::Message::Type::Fainted, isActive)}, true);
    queueCommand({cmd::Animation(isActive, cmd::Animation::Type::SlideDown)}, true);

    // total mom
    if (b.activePeoplemon().currentAbility() == pplmn::SpecialAbility::TotalMom) {
        b.getSubstate().totalMomIndex = b.outNowIndex();
    }
}

bool LocalBattleController::tryMidturnSwitch(Battler& switcher) {
    if (!canSwitch(switcher)) { return false; }

    midturnSwitcher = &switcher;
    setBattleState(BattleState::Stage::WaitingMidTurnSwitch);
    return true;
}

bool LocalBattleController::canSwitch(Battler& switcher) {
    const bool isActive = &switcher == &state->activeBattler();
    Battler& other      = isActive ? state->inactiveBattler() : state->activeBattler();

    // check trapped ailment
    if (switcher.getSubstate().hasAilment(pplmn::PassiveAilment::Trapped)) {
        queueCommand({cmd::Message(cmd::Message::Type::TrappedAilment, isActive)}, true);
        return false;
    }

    // check for board game master ability
    if (other.activePeoplemon().currentAbility() == pplmn::SpecialAbility::BoardGameMaster) {
        queueCommand({cmd::Message(cmd::Message::Type::BoardGameSwitchBlocked, isActive)}, true);
        return false;
    }

    return true;
}

float LocalBattleController::getEffectivenessMultiplier(pplmn::BattlePeoplemon& attacker,
                                                        pplmn::BattlePeoplemon& defender,
                                                        pplmn::MoveId move, pplmn::Type moveType) {
    const bool isActive = &attacker == &state->activeBattler().activePeoplemon();

    float e = pplmn::TypeUtil::getSuperMult(moveType, defender.base().type());

    // check engaging ability
    if (e < 0.1f && attacker.currentAbility() == pplmn::SpecialAbility::Engaging) {
        e = 0.5f;
        queueCommand({cmd::Message(cmd::Message::Type::EngagingAbility, move, isActive)}, true);
    }

    // check snapshot ability
    if (defender.currentAbility() == pplmn::SpecialAbility::Snapshot) {
        Battler& victim = isActive ? state->inactiveBattler() : state->activeBattler();
        if (victim.getSubstate().lastMoveSuperEffective == move && e > 1.1f) {
            e = 1.f;
            queueCommand({cmd::Message(cmd::Message::Type::SnapshotAbility, !isActive)}, true);
        }
    }

    // check absolute pitch ability
    if (attacker.currentAbility() == pplmn::SpecialAbility::AbsolutePitch) {
        if (e < 0.9f && e > 0.1f) {
            e *= 0.85f;
            queueCommand({cmd::Message(cmd::Message::Type::AbsPitchNotEffective, isActive)}, true);
        }
        else if (e > 1.1f) {
            e *= 1.15f;
            queueCommand({cmd::Message(cmd::Message::Type::AbsPitchSuperEffective, isActive)},
                         true);
        }
    }

    return e;
}

void LocalBattleController::checkKlutz(Battler& battler) {
    const bool isActive         = &battler == &state->activeBattler();
    pplmn::BattlePeoplemon& ppl = battler.activePeoplemon();

    if (ppl.currentAbility() == pplmn::SpecialAbility::Klutz) {
        if (bl::util::Random::get<int>(0, 100) <= 33) {
            const item::Id item   = ppl.base().holdItem();
            ppl.base().holdItem() = item::Id::None;
            // TODO - how to restore items to network player?
            if (&battler == &state->localPlayer()) { battle->player.state().bag.addItem(item); }
            queueCommand({cmd::Message(cmd::Message::Type::KlutzDrop, item, isActive)}, true);
        }
    }
}

bool LocalBattleController::teachThisTurn(Battler& battler) {
    return battler.chosenAction() == TurnAction::Fight && battler.chosenMove() >= 0 &&
           isTeachMove(battler.activePeoplemon().base().knownMoves()[battler.chosenMove()].id);
}

int LocalBattleController::getPriority(Battler& battler) {
    const bool isActive = &battler == &state->activeBattler();

    // check get baked ability
    if (battler.activePeoplemon().currentAbility() == pplmn::SpecialAbility::GetBaked &&
        battler.activePeoplemon().base().currentHp() <=
            battler.activePeoplemon().currentStats().hp / 5) {
        battler.getSubstate().gotBaked = true;
        queueCommand({cmd::Message(cmd::Message::Type::GetBakedAbility, isActive)}, true);
        return pplmn::Move::priority(pplmn::MoveId::SkimpOut);
    }

    int base = battler.getPriority();

    if (firstTurn) {
        // Check quick draw ability
        if (battler.activePeoplemon().currentAbility() == pplmn::SpecialAbility::QuickDraw &&
            battler.chosenAction() == TurnAction::Fight) {
            queueCommand({cmd::Message(cmd::Message::Type::QuickDrawFirst, isActive)}, true);
            return 1000000;
        }

        // Check new teach ability
        if (battler.activePeoplemon().currentAbility() == pplmn::SpecialAbility::NewTeach &&
            teachThisTurn(battler)) {
            ++base;
            Battler& other = isActive ? state->inactiveBattler() : state->activeBattler();
            if (other.chosenAction() == TurnAction::Fight) {
                queueCommand({cmd::Message(cmd::Message::Type::NewTeachAbility, isActive)}, true);
            }
        }
    }

    return base;
}

int LocalBattleController::getSpeed(Battler& battler) {
    const bool isActive         = &battler == &state->activeBattler();
    pplmn::BattlePeoplemon& ppl = battler.activePeoplemon();
    int spd                     = ppl.getSpeed();

    // check speed juice hold item
    if (ppl.base().holdItem() == item::Id::SpeedJuice &&
        ppl.base().currentHp() <= ppl.currentStats().hp / 4) {
        ppl.base().holdItem() = item::Id::None;
        spd                   = spd * 11 / 10;
        queueCommand({cmd::Message(cmd::Message::Type::SpeedJuice, isActive)}, true);
    }

    return spd;
}

void LocalBattleController::checkAbilitiesAfterMove(Battler& user) {
    const bool userIsActive = &user == &state->activeBattler();
    Battler& victim         = userIsActive ? state->inactiveBattler() : state->activeBattler();
    pplmn::BattlePeoplemon& attacker = user.activePeoplemon();
    pplmn::BattlePeoplemon& defender = victim.activePeoplemon();
    const bool contact               = pplmn::Move::makesContact(usedMove);
    const bool special               = pplmn::Move::isSpecial(usedMove);

    switch (defender.currentAbility()) {
    case pplmn::SpecialAbility::Goon:
        if (contact && damage > 0) {
            attacker.applyDamage(damage / 16);
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::GoonDamage, userIsActive)}, true);
        }
        break;

    case pplmn::SpecialAbility::Sassy:
        if (!contact && damage > 0) {
            attacker.applyDamage(damage / 16);
            queueCommand({Command::SyncStateNoSwitch});
            queueCommand({cmd::Message(cmd::Message::Type::SassyDamage, userIsActive)}, true);
        }
        break;

    case pplmn::SpecialAbility::DerpDerp:
        if (damage > 0 && bl::util::Random::get<int>(0, 100) <= 10) {
            user.getSubstate().giveAilment(pplmn::PassiveAilment::Confused);
            queueCommand({cmd::Message(cmd::Message::Type::DerpDerpConfuse, userIsActive)}, true);
        }
        break;

    case pplmn::SpecialAbility::DozeOff:
        if (isTeachMove(usedMove) && bl::util::Random::get<int>(0, 100) <= 10) {
            if (defender.giveAilment(pplmn::Ailment::Sleep)) {
                queueCommand({cmd::Message(cmd::Message::Type::DozeOffAbility, userIsActive)},
                             true);
            }
        }
        break;

    case pplmn::SpecialAbility::DouseFlames:
        if (!special && damage > 0 && bl::util::Random::get<int>(0, 100) <= 20) {
            queueCommand({cmd::Message(cmd::Message::Type::DouseFlamesAbility, userIsActive)},
                         true);
            doStatChange(attacker, pplmn::Stat::Attack, -1);
        }
        break;

    case pplmn::SpecialAbility::Flirty:
        if (special && damage > 0 && bl::util::Random::get<int>(0, 100) <= 20) {
            queueCommand({cmd::Message(cmd::Message::Type::FlirtyAbility, userIsActive)}, true);
            doStatChange(attacker, pplmn::Stat::SpecialAttack, -1);
        }
        break;

    case pplmn::SpecialAbility::CantSwim:
        if (damage > 0 && bl::util::Random::get<int>(0, 100) <= 15) {
            queueCommand({cmd::Message(cmd::Message::Type::CantSwimAbility, userIsActive)}, true);
            doStatChange(attacker, pplmn::Stat::Accuracy, -1);
        }
        break;

    case pplmn::SpecialAbility::GamemakerVirus:
        if (damage > 0) {
            pplmn::OwnedMove* move = attacker.base().findMove(usedMove);
            if (move) {
                move->curPP = (move->curPP) / 2 + (move->curPP % 2);
                queueCommand(
                    {cmd::Message(cmd::Message::Type::GameMakerVirusAbility, userIsActive)}, true);
            }
        }
        break;

    default:
        break;
    }

    switch (attacker.currentAbility()) {
    case pplmn::SpecialAbility::SidetrackTeach:
        if (isTeachMove(usedMove) && bl::util::Random::get<int>(0, 100) <= 15) {
            victim.getSubstate().giveAilment(pplmn::PassiveAilment::Distracted);
            queueCommand({cmd::Message(cmd::Message::Type::SidetrackDistract, userIsActive)}, true);
        }
        break;

    case pplmn::SpecialAbility::FieryTeach:
        if (isTeachMove(usedMove) && bl::util::Random::get<int>(0, 100) <= 33) {
            queueCommand({cmd::Message(cmd::Message::Type::FieryTeachAbility, userIsActive)}, true);
            doStatChange(attacker, pplmn::Stat::SpecialAttack, 1);
        }
        break;

    default:
        break;
    }

    // also check backwards hoodie confuse here
    if (defender.base().holdItem() == item::Id::BackwardsHoody && damage > 0) {
        if (bl::util::Random::get<int>(0, 100) <= 25) {
            if (!user.getSubstate().hasAilment(pplmn::PassiveAilment::Confused)) {
                user.getSubstate().giveAilment(pplmn::PassiveAilment::Confused);
                queueCommand(
                    {cmd::Message(cmd::Message::Type::BackwordsHoodyConfuse, userIsActive)}, true);
            }
        }
    }
}

void LocalBattleController::handleMoveEffect(Battler& user) {
    const bool userIsActive = &user == &state->activeBattler();
    Battler& victim         = userIsActive ? state->inactiveBattler() : state->activeBattler();
    pplmn::BattlePeoplemon& attacker = user.activePeoplemon();
    pplmn::BattlePeoplemon& defender = victim.activePeoplemon();

    const int chance = pplmn::Move::effectChance(usedMove);
    if (effect == pplmn::MoveEffect::None ||
        (chance > 0 && bl::util::Random::get<int>(0, 100) > chance))
        return;

    const bool affectsSelf           = pplmn::Move::affectsUser(usedMove);
    pplmn::BattlePeoplemon& affected = affectsSelf ? attacker : defender;
    Battler& affectedOwner           = affectsSelf ? user : victim;
    Battler& other                   = affectsSelf ? victim : user;
    const int intensity              = pplmn::Move::effectIntensity(usedMove);
    const bool forActive             = &affectedOwner == &state->activeBattler();

    // bail early if dead and the effect doesn't always apply
    if (affected.base().currentHp() == 0 && !doEvenIfDead(effect)) return;

    switch (effect) {
    case pplmn::MoveEffect::Heal:
        affected.giveHealth(affected.currentStats().hp * intensity / 100);
        queueCommand({cmd::Message(cmd::Message::Type::AttackRestoredHp, forActive)});
        break;

    case pplmn::MoveEffect::Absorb:
        affected.giveHealth(damage * intensity / 100);
        queueCommand({cmd::Message(cmd::Message::Type::Absorb, forActive)});
        break;

    case pplmn::MoveEffect::Poison:
        applyAilmentFromMove(affectedOwner, affected, pplmn::Ailment::Sticky);
        break;

    case pplmn::MoveEffect::Burn:
        applyAilmentFromMove(affectedOwner, affected, pplmn::Ailment::Frustrated);
        break;

    case pplmn::MoveEffect::Paralyze:
        applyAilmentFromMove(affectedOwner, affected, pplmn::Ailment::Annoyed);
        break;

    case pplmn::MoveEffect::Freeze:
        applyAilmentFromMove(affectedOwner, affected, pplmn::Ailment::Frozen);
        break;

    case pplmn::MoveEffect::Confuse:
        applyAilmentFromMove(affectedOwner, affected, pplmn::PassiveAilment::Confused);
        break;

    case pplmn::MoveEffect::LeechSeed:
        applyAilmentFromMove(affectedOwner, affected, pplmn::PassiveAilment::Stolen);
        break;

    case pplmn::MoveEffect::Flinch:
        if (state->isFirstMover()) {
            affectedOwner.getSubstate().giveAilment(pplmn::PassiveAilment::Distracted);
            queueCommand({cmd::Message(cmd::Message::Type::GainedPassiveAilment,
                                       pplmn::PassiveAilment::Distracted,
                                       forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::PassiveAilmentGiveFail,
                                       pplmn::PassiveAilment::Distracted,
                                       forActive)});
        }
        break;

    case pplmn::MoveEffect::Trap:
        applyAilmentFromMove(affectedOwner, affected, pplmn::PassiveAilment::Trapped);
        break;

    case pplmn::MoveEffect::Sleep:
        applyAilmentFromMove(affectedOwner, affected, pplmn::Ailment::Sleep);
        break;

    case pplmn::MoveEffect::Protection:
        if (state->isFirstMover() && user.getSubstate().lastMoveUsed != usedMove) {
            user.getSubstate().isProtected = true;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::GenericMoveFailed)});
        }
        break;

    case pplmn::MoveEffect::Safegaurd:
        if (affectedOwner.getSubstate().turnsGuarded == 0) {
            affectedOwner.getSubstate().turnsGuarded = 5;
            queueCommand({cmd::Message(cmd::Message::Type::Guarded, forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::GuardFailed, forActive)});
        }
        break;

    case pplmn::MoveEffect::Substitute:
        affected.applyDamage(affected.currentStats().hp / 4);
        if (affected.base().currentHp() == 0) {
            queueCommand({cmd::Message(cmd::Message::Type::SubstituteSuicide, forActive)});
        }
        else if (affectedOwner.getSubstate().substituteHp > 0) {
            queueCommand({cmd::Message(cmd::Message::Type::SubstituteAlreadyExists, forActive)});
        }
        else {
            affectedOwner.getSubstate().substituteHp = affected.currentStats().hp / 4;
            queueCommand({cmd::Message(cmd::Message::Type::SubstituteCreated, forActive)});
        }
        break;

    case pplmn::MoveEffect::HealBell: {
        bool healed = false;
        for (pplmn::BattlePeoplemon& ppl : affectedOwner.peoplemon()) {
            if (ppl.clearAilments(&affectedOwner.getSubstate())) { healed = true; }
        }
        if (healed) { queueCommand({cmd::Message(cmd::Message::Type::HealBellHealed, forActive)}); }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::HealBellAlreadyHealthy, forActive)});
        }
    } break;

    case pplmn::MoveEffect::CritUp:
        doStatChange(affected, pplmn::Stat::Critical, intensity);
        break;

    case pplmn::MoveEffect::AtkUp:
        doStatChange(affected, pplmn::Stat::Attack, intensity);
        break;

    case pplmn::MoveEffect::DefUp:
        doStatChange(affected, pplmn::Stat::Defense, intensity);
        break;

    case pplmn::MoveEffect::AccUp:
        doStatChange(affected, pplmn::Stat::Accuracy, intensity);
        break;

    case pplmn::MoveEffect::EvdUp:
        doStatChange(affected, pplmn::Stat::Evasion, intensity);
        break;

    case pplmn::MoveEffect::SpdUp:
        doStatChange(affected, pplmn::Stat::Speed, intensity);
        break;

    case pplmn::MoveEffect::SpAtkUp:
        doStatChange(affected, pplmn::Stat::SpecialAttack, intensity);
        break;

    case pplmn::MoveEffect::SpDefUp:
        doStatChange(affected, pplmn::Stat::SpecialDefense, intensity);
        break;

    case pplmn::MoveEffect::CritDown:
        doStatChange(affected, pplmn::Stat::Critical, -intensity);
        break;

    case pplmn::MoveEffect::AtkDown:
        doStatChange(affected, pplmn::Stat::Attack, -intensity);
        break;

    case pplmn::MoveEffect::DefDown:
        doStatChange(affected, pplmn::Stat::Defense, -intensity);
        break;

    case pplmn::MoveEffect::AccDown:
        doStatChange(affected, pplmn::Stat::Accuracy, -intensity);
        break;

    case pplmn::MoveEffect::EvdDown:
        doStatChange(affected, pplmn::Stat::Evasion, -intensity);
        break;

    case pplmn::MoveEffect::SpdDown:
        doStatChange(affected, pplmn::Stat::Speed, -intensity);
        break;

    case pplmn::MoveEffect::SpAtkDown:
        doStatChange(affected, pplmn::Stat::SpecialAttack, -intensity);
        break;

    case pplmn::MoveEffect::SpDefDown:
        doStatChange(affected, pplmn::Stat::SpecialDefense, -intensity);
        break;

    case pplmn::MoveEffect::CritEvdUp:
        queueCommand({cmd::Animation(forActive, cmd::Animation::Type::MultipleStateIncrease)},
                     true);
        doStatChange(affected, pplmn::Stat::Evasion, intensity, false);
        doStatChange(affected, pplmn::Stat::Critical, intensity, false);
        break;

    case pplmn::MoveEffect::SpdAtkUp:
        queueCommand({cmd::Animation(forActive, cmd::Animation::Type::MultipleStateIncrease)},
                     true);
        doStatChange(affected, pplmn::Stat::Attack, intensity, false);
        doStatChange(affected, pplmn::Stat::Speed, intensity, false);
        break;

    case pplmn::MoveEffect::Recoil:
        affected.applyDamage(damage * intensity / 100);
        queueCommand({cmd::Message(cmd::Message::Type::RecoilDamage, forActive)});
        break;

    case pplmn::MoveEffect::Suicide:
        affected.base().currentHp() = 0;
        queueCommand({cmd::Message(cmd::Message::Type::SuicideEffect, forActive)});
        break;

    case pplmn::MoveEffect::Peanut:
        if (isPeanutAllergic(affected.base().id())) {
            affected.base().currentHp() = 0;
            queueCommand({cmd::Message(cmd::Message::Type::PeanutAllergic, forActive)});
        }
        else {
            affected.giveHealth(1);
            queueCommand({cmd::Message(cmd::Message::Type::PeanutAte, forActive)});
        }
        break;

    case pplmn::MoveEffect::WakeBoth:
        for (pplmn::BattlePeoplemon& ppl : state->localPlayer().peoplemon()) {
            if (ppl.base().currentAilment() == pplmn::Ailment::Sleep) {
                ppl.base().currentAilment() = pplmn::Ailment::None;
            }
        }
        for (pplmn::BattlePeoplemon& ppl : state->enemy().peoplemon()) {
            if (ppl.base().currentAilment() == pplmn::Ailment::Sleep) {
                ppl.base().currentAilment() = pplmn::Ailment::None;
            }
        }
        queueCommand({cmd::Message(cmd::Message::Type::EveryoneWokenUp)});
        break;

    case pplmn::MoveEffect::Encore:
        queueCommand({cmd::Message(cmd::Message::Type::EncoreStart, forActive)}, true);
        if (affectedOwner.getSubstate().lastMoveIndex >= 0) {
            affectedOwner.getSubstate().encoreTurnsLeft = 5;
            affectedOwner.getSubstate().encoreMove      = affectedOwner.getSubstate().lastMoveIndex;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::EncoreFailed, forActive)}, true);
        }
        break;

    case pplmn::MoveEffect::BatonPass:
        if (affectedOwner.canSwitch()) {
            if (tryMidturnSwitch(affectedOwner)) {
                affectedOwner.getSubstate().copyStatsFrom = affectedOwner.outNowIndex();
                queueCommand({cmd::Message(cmd::Message::Type::BatonPassStart, forActive)}, true);
            }
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::BatonPassFailed, forActive)});
        }
        break;

    case pplmn::MoveEffect::DieIn3Turns: {
        bool marked = false;
        if (state->activeBattler().getSubstate().deathCounter < 0) {
            queueCommand({cmd::Message(cmd::Message::Type::DeathCountDown, true)}, true);
            state->activeBattler().getSubstate().deathCounter = 3;
            marked                                            = true;
        }
        if (state->inactiveBattler().getSubstate().deathCounter < 0) {
            queueCommand({cmd::Message(cmd::Message::Type::DeathCountDown, false)}, true);
            state->inactiveBattler().getSubstate().deathCounter = 3;
            marked                                              = true;
        }
        if (!marked) {
            queueCommand({cmd::Message(cmd::Message::Type::DeathCountDownFailed, userIsActive)});
        }
    } break;

    case pplmn::MoveEffect::SetBall:
        if (affectedOwner.getSubstate().ballUpTurns >= 0) {
            queueCommand({cmd::Message(cmd::Message::Type::BallSet, forActive)}, true);
            affectedOwner.getSubstate().ballSet = true;
            if (affectedOwner.canSwitch()) { tryMidturnSwitch(affectedOwner); }
            else {
                affectedOwner.getSubstate().noOneToGetBall = true;
            }
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::BallSetFail, forActive)});
        }
        break;

    case pplmn::MoveEffect::BumpBall:
        if (affectedOwner.getSubstate().ballUpTurns < 0) {
            queueCommand({cmd::Message(cmd::Message::Type::BallServed, forActive)}, true);
            affectedOwner.getSubstate().ballUpTurns = 0;
            affectedOwner.getSubstate().ballBumped  = true;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::BallBumped, forActive)}, true);
            affectedOwner.getSubstate().ballBumped = true;
        }
        if (affectedOwner.canSwitch()) { tryMidturnSwitch(affectedOwner); }
        else {
            affectedOwner.getSubstate().noOneToGetBall = true;
        }
        break;

    case pplmn::MoveEffect::SpikeBall:
        if (affectedOwner.getSubstate().ballUpTurns >= 0) {
            queueCommand({cmd::Message(cmd::Message::Type::BallSpiked, forActive)});
            affectedOwner.getSubstate().ballSpiked = true;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::BallSpikeFail, forActive)});
        }
        break;

    case pplmn::MoveEffect::BlockBall:
        if (other.getSubstate().ballUpTurns >= 0) {
            queueCommand({cmd::Message(cmd::Message::Type::BallBlocked, forActive)});
            other.getSubstate().ballBlocked = true;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::BallBlockFail, forActive)});
        }
        break;

    case pplmn::MoveEffect::SwipeBall:
        if (affectedOwner.getSubstate().ballUpTurns >= 0) {
            queueCommand({cmd::Message(cmd::Message::Type::BallSwiped, forActive)});
            affectedOwner.getSubstate().ballSwiped = true;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::BallSwipeFail, forActive)});
        }
        break;

    case pplmn::MoveEffect::DeathSwap: {
        bool oneDead = false;
        for (const pplmn::BattlePeoplemon& p : affectedOwner.peoplemon()) {
            if (p.base().currentHp() == 0 && &p != &affected) {
                oneDead = true;
                break;
            }
        }
        if (oneDead) {
            affectedOwner.getSubstate().koReviveHp = affected.base().currentHp();
            affected.base().currentHp()            = 0;
            queueCommand({cmd::Message(cmd::Message::Type::DeathSwapSac, forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::DeathSwapFailed, forActive)});
        }
    } break;

    case pplmn::MoveEffect::StayAlive:
        if (!affectedOwner.getSubstate().enduredLastTurn) {
            affectedOwner.getSubstate().enduringThisTurn = true;
            queueCommand({cmd::Message(cmd::Message::Type::EndureStart, forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::EndureFail, forActive)});
        }
        break;

    case pplmn::MoveEffect::MaxAtkMinAcc:
        affected.statChange(pplmn::Stat::Attack, 12);
        affected.statChange(pplmn::Stat::Accuracy, -12);
        queueCommand({cmd::Message(cmd::Message::Type::MaxAtkMinAcc, forActive)});
        break;

    case pplmn::MoveEffect::FrustConfuse:
        applyAilmentFromMove(affectedOwner, affected, pplmn::Ailment::Frustrated);
        applyAilmentFromMove(affectedOwner, affected, pplmn::PassiveAilment::Confused);
        break;

    case pplmn::MoveEffect::Spikes:
        if (affectedOwner.getSubstate().spikesOut < 3) {
            affectedOwner.getSubstate().spikesOut += 1;
            queueCommand({cmd::Message(cmd::Message::Type::SpikesApplied, forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::SpikesFailed, forActive)});
        }
        break;

    case pplmn::MoveEffect::EnemyPPDown:
        if (!state->isFirstMover() && other.getSubstate().lastMoveIndex >= 0) {
            auto& move =
                other.activePeoplemon().base().knownMoves()[other.getSubstate().lastMoveIndex];
            move.curPP =
                std::max(0, static_cast<int>(move.curPP) - bl::util::Random::get<int>(2, 5));
            queueCommand({cmd::Message(cmd::Message::Type::PPLowered, forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::PPLowerFail, forActive)}, true);
        }
        break;

    case pplmn::MoveEffect::HealNext:
        if (affectedOwner.getSubstate().healNext < 0) {
            affectedOwner.getSubstate().healNext = 0;
            queueCommand({cmd::Message(cmd::Message::Type::HealNextStart, forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::HealNextFail, forActive)});
        }
        break;

    case pplmn::MoveEffect::Roar:
        doRoar(affectedOwner);
        break;

    case pplmn::MoveEffect::RoarCancelBallSpikes:
        queueCommand({cmd::Message(cmd::Message::Type::RoarClearedArea, !forActive)}, true);
        other.getSubstate().spikesOut           = 0;
        affectedOwner.getSubstate().ballUpTurns = -1;
        doRoar(affectedOwner);
        break;

    case pplmn::MoveEffect::SleepHeal:
        if (affected.base().currentAilment() == pplmn::Ailment::None) {
            affected.base().currentAilment() = pplmn::Ailment::Sleep;
            affected.clearAilments(&affectedOwner.getSubstate());
            affected.base().currentHp() = affected.currentStats().hp;
            queueCommand({cmd::Message(cmd::Message::Type::SleepHealed, forActive)});
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::SleepHealFailed, forActive)});
        }
        break;

    case pplmn::MoveEffect::StealStats:
        affected.copyStages(other.activePeoplemon());
        other.activePeoplemon().resetStages();
        queueCommand({cmd::Message(cmd::Message::Type::StatsStolen, forActive)});
        break;

    case pplmn::MoveEffect::DamageThenSwitch:
        if (affectedOwner.canSwitch()) {
            if (tryMidturnSwitch(affectedOwner)) {
                queueCommand({cmd::Message(cmd::Message::Type::AttackThenSwitched, forActive)},
                             true);
            }
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::AttackSwitchFailed, forActive)});
        }
        break;

    case pplmn::MoveEffect::OnlySleeping:
    case pplmn::MoveEffect::Charge:
    case pplmn::MoveEffect::FailOnMove64:
        // handled in checkMoveCancelled
        break;

    case pplmn::MoveEffect::Gamble:
    case pplmn::MoveEffect::RandomMove:
    case pplmn::MoveEffect::DoubleFamily:
    case pplmn::MoveEffect::Counter:
    case pplmn::MoveEffect::MirrorCoat:
        // handled up top
        break;

    default:
        BL_LOG_ERROR << "Unknown move effect: " << effect;
        break;
    }

    queueCommand({Command::WaitForView});
}

} // namespace battle
} // namespace core
