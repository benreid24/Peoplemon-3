#include <Core/Battles/BattleControllers/LocalBattleController.hpp>

#include <Core/Battles/Battle.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Battles/BattleView.hpp>

#ifdef PEOPLEMON_DEBUG
#define BATTLE_LOG BL_LOG_INFO
#else
#define BATTLE_LOG \
    if constexpr (false) BL_LOG_INFO
#endif

namespace core
{
namespace battle
{
LocalBattleController::LocalBattleController()
: currentStageInitialized(false) {}

void LocalBattleController::updateBattleState(bool viewSynced, bool queueEmpty) {
    if (!currentStageInitialized) {
        initCurrentStage();
        currentStageInitialized = true;
    }
    else {
        checkCurrentStage(viewSynced, queueEmpty);
    }
}

void LocalBattleController::initCurrentStage() {
    using Stage = BattleState::Stage;

    BATTLE_LOG << "Initializing battle stage: " << state->currentStage();

    switch (state->currentStage()) {
    case Stage::WildIntro:
        // TODO - play anim
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::WildIntro, state->enemy().name())});
        queueCommand({Command::WaitForView});
        break;

    case Stage::TrainerIntro:
        // TODO - show image somehow
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::TrainerIntro, state->enemy().name())});
        queueCommand({Command::WaitForView});
        break;

    case Stage::NetworkIntro:
        // TODO - show image?
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::NetworkIntro, state->enemy().name())});
        queueCommand({Command::WaitForView});
        break;

    case Stage::IntroSendInSelf:
        queueCommand({cmd::Message(cmd::Message::Type::PlayerFirstSendout)});
        queueCommand({cmd::Animation(cmd::Animation::Target::User,
                                     cmd::Animation::Type::PlayerFirstSendout)});
        queueCommand({Command::WaitForView});
        break;

    case Stage::IntroSendInOpponent:
        queueCommand({cmd::Message(cmd::Message::Type::OpponentFirstSendout)});
        queueCommand({cmd::Animation(cmd::Animation::Target::User,
                                     cmd::Animation::Type::OpponentFirstSendout)});
        queueCommand({Command::WaitForView});
        break;

    case Stage::WaitingChoices:
        queueCommand({Command::GetBattlerChoices});
        break;

    case Stage::PreUseItem:
        // TODO - display message
        break;

    case Stage::UsingItem:
        // TODO - apply item and sync view
        break;

    case Stage::PostUseItem:
        // TODO - display message of effect
        break;

    case Stage::BeforeSwitch:
        // TODO - display message
        break;

    case Stage::Switching:
        // TODO - play anim sequence
        break;

    case Stage::AfterSwitch:
        // TODO - check abilities and stuff
        break;

    case Stage::BeforeRun:
        // TODO - show message and determine result
        break;

    case Stage::Running:
        // TODO - display message
        break;

    case Stage::RunFailed:
        // TODO - display message
        break;

    case Stage::BeforeAttack: {
        // TODO - check abilities etc, show message
        const pplmn::MoveId mid = state->activeBattler()
                                      .activePeoplemon()
                                      .base()
                                      .knownMoves()[state->activeBattler().chosenMove()]
                                      .id;
        queueCommand({cmd::Message(cmd::Message(mid))});
        queueCommand({Command::WaitForView});
        queueCommand({cmd::Animation(cmd::Animation::Target::Other, mid)});
        queueCommand({Command::WaitForView});
        queueCommand({cmd::Message(cmd::Message::Type::SuperEffective)});
        queueCommand({Command::WaitForView});
        // TODO - do we need 3 separate attack phases?
    } break;

    case Stage::Attacking:
        // TODO - play animations and apply move
        break;

    case Stage::AfterAttack:
        // TODO - sync bars, check abilities, do stat mods and anims
        break;

    case Stage::BeforeFaint:
        // TODO - display message
        break;

    case Stage::Fainting:
        // TODO - play anim
        break;

    case Stage::XpAwarding:
        // TODO - add xp and sync bars
        break;

    case Stage::LevelingUp:
        // TODO - increase level, show level up box
        // TODO - determine best way to defer evolution to after battle
        break;

    case Stage::BeforeFaintSwitch:
        // TODO - ask to continue if wild peoplemon
        break;

    case Stage::FaintSwitching:
        // TODO - display animation
        break;

    case Stage::AfterFaintSwitch:
        // TODO - sync boxes
        break;

    case Stage::TrainerDefeated:
        // TODO - show message. give money
        break;

    case Stage::PeoplemonCaught:
        // TODO - display message, give peoplemon
        break;

    case Stage::NetworkDefeated:
        // TODO - display message
        break;

    case Stage::NetworkLost:
        // TODO - display message
        break;

    case Stage::LocalLost:
        // TODO - show message
        break;

    case Stage::NetworkDisconnect:
    case Stage::NextBattler:
    case Stage::Completed:
    case Stage::TurnStart:
    case Stage::HandleFaint:
    case Stage::RoundStart:
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
        BATTLE_LOG << "Detected end of stage " << state->currentStage();

        switch (state->currentStage()) {
        case Stage::WildIntro:
            setBattleState(Stage::IntroSendInOpponent);
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

        case Stage::PreUseItem:
            setBattleState(Stage::UsingItem);
            break;

        case Stage::UsingItem:
            setBattleState(Stage::PostUseItem);
            break;

        case Stage::PostUseItem:
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

        case Stage::BeforeAttack:
            setBattleState(Stage::Attacking);
            break;

        case Stage::Attacking:
            setBattleState(Stage::AfterAttack);
            break;

        case Stage::AfterAttack:
            if (state->inactiveBattler().activePeoplemon().base().currentHp() == 0) {
                setBattleState(Stage::BeforeFaint);
            }
            else {
                setBattleState(Stage::NextBattler);
            }
            break;

        case Stage::BeforeFaint:
            setBattleState(Stage::Fainting);
            break;

        case Stage::Fainting:
            if (&state->activeBattler() == &state->localPlayer() &&
                battle->type != Battle::Type::Online) {
                setBattleState(Stage::XpAwarding);
            }
            else {
                setBattleState(Stage::HandleFaint);
            }
            break;

        case Stage::XpAwarding:
            // TODO - determine how xp is carried over and leveling is done
            setBattleState(Stage::HandleFaint);
            break;

        case Stage::LevelingUp:
            setBattleState(Stage::XpAwarding);
            break;

        case Stage::BeforeFaintSwitch:
            if (state->inactiveBattler().actionSelected()) {
                setBattleState(Stage::FaintSwitching);
            }
            break;

        case Stage::FaintSwitching:
            setBattleState(Stage::AfterFaintSwitch);
            break;

        case Stage::AfterFaintSwitch:
            setBattleState(Stage::WaitingChoices);
            break;

        case Stage::TrainerDefeated:
        case Stage::NetworkDefeated:
        case Stage::NetworkLost:
        case Stage::LocalLost:
        case Stage::PeoplemonCaught:
            setBattleState(Stage::Completed);
            break;

        case Stage::NetworkDisconnect:
        case Stage::NextBattler:
        case Stage::Completed:
        case Stage::HandleFaint:
        case Stage::RoundStart:
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

    BATTLE_LOG << "Switching to stage: " << ns;

    Stage nns = getNextStage(ns);
    while (nns != ns) {
        BATTLE_LOG << "Immediate stage switch: " << ns << " -> " << nns;
        ns  = nns;
        nns = getNextStage(nns);
    }

    BATTLE_LOG << "Final stage transition: " << state->currentStage() << " -> " << ns;

    state->setStage(ns);
    currentStageInitialized = false;
}

BattleState::Stage LocalBattleController::getNextStage(BattleState::Stage ns) {
    using Stage = BattleState::Stage;

    switch (ns) {
    case Stage::NextBattler: {
        const BattleState::Stage next = state->nextTurn();
        queueCommand({Command::SyncStateNoSwitch});
        return next;
    }

    case Stage::HandleFaint:
        // TODO - determine if victory or switch
        return Stage::BeforeFaintSwitch;

    case Stage::TurnStart:
        switch (state->activeBattler().chosenAction()) {
        case TurnAction::Fight:
            return Stage::BeforeAttack;
        case TurnAction::Item:
            return Stage::PreUseItem;
        case TurnAction::Run:
            return Stage::BeforeRun;
        case TurnAction::Switch:
            return Stage::BeforeSwitch;
        default:
            BL_LOG_ERROR << "Unknown turn action: " << state->activeBattler().chosenAction();
            return Stage::Completed;
        }

    case Stage::RoundStart:
        state->beginRound(state->localPlayer().getPriority() >= state->enemy().getPriority());
        queueCommand({Command::SyncStateNoSwitch});
        return Stage::TurnStart;

        // TODO - other special cases

    default:
        return ns;
    }
}

void LocalBattleController::onCommandQueued(const Command&) {
    // TODO - anything?
}

void LocalBattleController::onCommandProcessed(const Command&) {
    // TODO - anything?
}

void LocalBattleController::onUpdate(bool vs, bool qe) { updateBattleState(vs, qe); }

} // namespace battle
} // namespace core
