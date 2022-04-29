#include <Core/Battles/BattleControllers/LocalBattleController.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Battles/Battle.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Battles/BattleView.hpp>
#include <Core/Peoplemon/Move.hpp>

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
namespace
{
int critChance(int stage) {
    switch (stage) {
    case 0:
        return 7;
    case 1:
        return 13;
    case 2:
        return 50;
    default:
        return 100; // this seems wrong
    }
}
} // namespace

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
        queueCommand({cmd::Message(cmd::Message::Type::WildIntro, state->enemy().name())}, true);
        break;

    case Stage::TrainerIntro:
        // TODO - show image somehow
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::TrainerIntro, state->enemy().name())}, true);
        break;

    case Stage::NetworkIntro:
        // TODO - show image?
        queueCommand({Command::SyncStateNoSwitch});
        queueCommand({cmd::Message(cmd::Message::Type::NetworkIntro, state->enemy().name())}, true);
        break;

    case Stage::IntroSendInSelf:
        queueCommand({cmd::Message(cmd::Message::Type::PlayerFirstSendout)});
        queueCommand({cmd::Animation(cmd::Animation::Target::User,
                                     cmd::Animation::Type::PlayerFirstSendout)},
                     true);
        break;

    case Stage::IntroSendInOpponent:
        queueCommand({cmd::Message(cmd::Message::Type::OpponentFirstSendout)});
        queueCommand({cmd::Animation(cmd::Animation::Target::User,
                                     cmd::Animation::Type::OpponentFirstSendout)},
                     true);
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

    case Stage::Attacking:
        startUseMove(state->activeBattler(), state->activeBattler().chosenMove());
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

        case Stage::Attacking:
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
            return Stage::Attacking;
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

void LocalBattleController::startUseMove(Battler& user, int index) {
    static pplmn::OwnedMove flail(pplmn::MoveId::SuperTrustFall); // TODO - replace with correct id

    Battler& victim = &user == &state->localPlayer() ? state->enemy() : state->localPlayer();
    pplmn::BattlePeoplemon& attacker = user.activePeoplemon();
    pplmn::BattlePeoplemon& defender = victim.activePeoplemon();

    pplmn::OwnedMove& move = index >= 0 ? user.activePeoplemon().base().knownMoves()[index] : flail;
    move.curPP -= 1;

    queueCommand({cmd::Message(cmd::Message(move.id))}, true);

    // determine if hit
    const int acc       = pplmn::Move::accuracy(move.id);
    const int pacc      = attacker.battleStats().acc;
    const int evd       = defender.battleStats().evade;
    const int hitChance = acc * pacc / evd;
    if (bl::util::Random::get<int>(0, 100) > hitChance && acc != 0) {
        queueCommand({cmd::Message::Type::AttackMissed}, true);
        return;
    }

    // move has hit
    // TODO - will have to move these in event that move gets canceled
    queueCommand({cmd::Animation(cmd::Animation::Target::Other, move.id)}, true);

    // determine if attacking move
    const pplmn::Type moveType = pplmn::Move::type(move.id);
    const bool special         = pplmn::Move::isSpecial(move.id);
    int pwr                    = pplmn::Move::damage(move.id);

    // move does damage
    if (pwr > 0) {
        // TODO - move may not hit based on abilities and prior moves, may need to move these
        queueCommand(
            {cmd::Animation(cmd::Animation::Target::Other, cmd::Animation::Type::ShakeAndFlash)});
        queueCommand({Command::SyncStateNoSwitch}, true);

        int atk          = special ? attacker.currentStats().spatk : attacker.currentStats().atk;
        int def          = special ? defender.currentStats().spdef : defender.currentStats().def;
        const float stab = pplmn::TypeUtil::getStab(moveType, attacker.base().type());
        const float effective  = pplmn::TypeUtil::getSuperMult(moveType, defender.base().type());
        const float multiplier = bl::util::Random::get<float>(0.85f, 1.f);
        const bool crit =
            bl::util::Random::get<int>(0, 100) <= critChance(attacker.battleStats().crit);
        int damage = ((2 * attacker.base().currentLevel() + 10) / 250) * (atk / def) * pwr + 2;
        damage *= stab * multiplier * effective * (crit ? 2.f : 1.f);

        BATTLE_LOG << pplmn::Move::name(move.id) << " did " << damage << " damage to "
                   << defender.base().name();
        defender.applyDamage(damage);

        if (crit) { queueCommand({cmd::Message(cmd::Message::Type::CriticalHit)}, true); }
        if (effective > 1.1f) {
            defender.notifySuperEffectiveHit(move.id);
            queueCommand({cmd::Message(cmd::Message::Type::SuperEffective)}, true);
        }
        else if (effective < 0.1f) {
            queueCommand({cmd::Message(cmd::Message::Type::IsNotAffected)}, true);
            return;
        }
        else {
            queueCommand({cmd::Message(cmd::Message::Type::NotEffective)}, true);
        }
    }

    // resolve move effect if any
    const pplmn::MoveEffect effect = pplmn::Move::effect(move.id);
    const int chance               = pplmn::Move::effectChance(move.id);
    if (effect != pplmn::MoveEffect::None &&
        (chance < 0 || bl::util::Random::get<int>(0, 100) < chance)) {
        const bool affectsSelf           = pplmn::Move::affectsUser(move.id);
        pplmn::BattlePeoplemon& affected = affectsSelf ? attacker : defender;
        const int intensity              = pplmn::Move::effectIntensity(move.id);

        switch (effect) {
        case pplmn::MoveEffect::Heal:
            break;
        case pplmn::MoveEffect::Poison:
            break;
        case pplmn::MoveEffect::Burn:
            break;
        case pplmn::MoveEffect::Paralyze:
            break;
        case pplmn::MoveEffect::Freeze:
            break;
        case pplmn::MoveEffect::Confuse:
            break;
        case pplmn::MoveEffect::LeechSeed:
            break;
        case pplmn::MoveEffect::Flinch:
            break;
        case pplmn::MoveEffect::Trap:
            break;
        case pplmn::MoveEffect::Sleep:
            break;
        case pplmn::MoveEffect::Protection:
            break;
        case pplmn::MoveEffect::Safegaurd:
            break;
        case pplmn::MoveEffect::Substitute:
            break;
        case pplmn::MoveEffect::HealBell:
            break;
        case pplmn::MoveEffect::CritUp:
            break;
        case pplmn::MoveEffect::AtkUp:
            break;
        case pplmn::MoveEffect::DefUp:
            break;
        case pplmn::MoveEffect::AccUp:
            break;
        case pplmn::MoveEffect::EvdUp:
            break;
        case pplmn::MoveEffect::SpdUp:
            break;
        case pplmn::MoveEffect::SpAtkUp:
            break;
        case pplmn::MoveEffect::SpDefUp:
            break;
        case pplmn::MoveEffect::CritDown:
            break;
        case pplmn::MoveEffect::AtkDown:
            break;
        case pplmn::MoveEffect::DefDown:
            break;
        case pplmn::MoveEffect::AccDown:
            break;
        case pplmn::MoveEffect::EvdDown:
            break;
        case pplmn::MoveEffect::SpdDown:
            break;
        case pplmn::MoveEffect::SpAtkDown:
            break;
        case pplmn::MoveEffect::SpDefDown:
            break;
        case pplmn::MoveEffect::Recoil:
            break;
        case pplmn::MoveEffect::Charge:
            break;
        case pplmn::MoveEffect::Suicide:
            break;
        case pplmn::MoveEffect::Counter:
            break;
        case pplmn::MoveEffect::MirrorCoat:
            break;
        case pplmn::MoveEffect::OnlySleeping:
            break;
        case pplmn::MoveEffect::Peanut:
            break;
        case pplmn::MoveEffect::SetBall:
            break;
        case pplmn::MoveEffect::WakeBoth:
            break;
        case pplmn::MoveEffect::HealPercent:
            break;
        case pplmn::MoveEffect::Encore:
            break;
        case pplmn::MoveEffect::RandomMove:
            break;
        case pplmn::MoveEffect::BatonPass:
            break;
        case pplmn::MoveEffect::DieIn3Turns:
            break;
        case pplmn::MoveEffect::CritEvdUp:
            break;
        case pplmn::MoveEffect::BumpBall:
            break;
        case pplmn::MoveEffect::SpikeBall:
            break;
        case pplmn::MoveEffect::DeathSwap:
            break;
        case pplmn::MoveEffect::Gamble:
            break;
        case pplmn::MoveEffect::StayAlive:
            break;
        case pplmn::MoveEffect::MaxAtkMinAcc:
            break;
        case pplmn::MoveEffect::FrustConfuse:
            break;
        case pplmn::MoveEffect::Spikes:
            break;
        case pplmn::MoveEffect::DoubleFamily:
            break;
        case pplmn::MoveEffect::EnemyPPDown:
            break;
        case pplmn::MoveEffect::HealNext:
            break;
        case pplmn::MoveEffect::Roar:
            break;
        case pplmn::MoveEffect::FailOnMove64:
            break;
        case pplmn::MoveEffect::SleepHeal:
            break;
        case pplmn::MoveEffect::SpdAtkUp:
            break;
        case pplmn::MoveEffect::StealStats:
            break;
        case pplmn::MoveEffect::BlockBall:
            break;
        case pplmn::MoveEffect::SwipeBall:
            break;
        case pplmn::MoveEffect::DamageThenSwitch:
            break;
        case pplmn::MoveEffect::RoarCancelBallSpikes:
            break;
        default:
            BATTLE_LOG << "Unknown move effect: " << effect;
            break;
        }
    }
}

} // namespace battle
} // namespace core
