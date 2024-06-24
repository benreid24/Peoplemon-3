#include <Core/Battles/View/MessagePrinter.hpp>

#include <BLIB/Render/Primitives/Color.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Battles/BattleState.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
const glm::vec2 TextPos(12.f, 473.f);
constexpr float TextWidth = 470.f;
const glm::vec2 ArrowPos(468.f, 573.f);
constexpr float TriangleFlashOn  = 0.75f;
constexpr float TriangleFlashOff = 0.65f;

using Message = cmd::Message;

std::string ailmentBlockedSuffix(Message::Type tp, const std::string& ppl) {
    switch (tp) {
    case Message::Type::AilmentSatPassiveAilmentBlocked:
        return " but " + ppl + " is Ailment Saturated!";
    case Message::Type::SubstituteAilmentBlocked:
    case Message::Type::SubstitutePassiveAilmentBlocked:
        return " but it's Substitute blocks ailments!";
    case Message::Type::GuardBlockedAilment:
    case Message::Type::GuardBlockedPassiveAilment:
        return " but it is Guarded!";
    case Message::Type::PassiveAilmentGiveFail:
    case Message::Type::AilmentGiveFail:
    default:
        return " but it failed!";
    }
}

std::string snackShareSuffix(Message::Type type, const std::string& other) {
    switch (type) {
    case Message::Type::GainedAilmentSnackshare:
        return " because of " + other + "'s Snack Share!";
    case Message::Type::GainedAilment:
    default:
        return "!";
    }
}
} // namespace

MessagePrinter::MessagePrinter(bl::engine::Engine& engine)
: engine(engine)
, state(State::Hidden)
, menu(0.f)
, keyboard(engine, std::bind(&MessagePrinter::nameEntered, this)) {}

void MessagePrinter::init(bl::rc::scene::CodeScene* scene) {
    menu.create(engine,
                engine.renderer().getObserver(),
                bl::menu::ArrowSelector::create(8.f, sf::Color::Black));

    triangle.create(engine, {0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f});
    triangle.getTransform().setPosition(ArrowPos);
    triangle.setFillColor(bl::sfcol(sf::Color(242, 186, 17)));
    triangle.flash(TriangleFlashOn, TriangleFlashOff);

    text.create(engine, Properties::MenuFont(), "", 18, {0.f, 0.f, 0.f, 1.f});
    text.getTransform().setPosition(TextPos);
    text.wordWrap(TextWidth);

    inputDriver.drive(&menu);
    yesItem = bl::menu::TextItem::create("Yes", Properties::MenuFont(), sf::Color::Black, 16);
    auto no = bl::menu::TextItem::create("No", Properties::MenuFont(), sf::Color::Black, 16);
    menu.setRootItem(yesItem);
    menu.addItem(no, yesItem.get(), bl::menu::Item::Bottom);
    menu.configureBackground(sf::Color::White, sf::Color::Black, 2.f, {14.f, 10.f, 0.f, 0.f});
    menu.setPosition({490.f - menu.getBounds().width * 2.f, 450.f - menu.getBounds().height * 2.f});

    yesItem->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&MessagePrinter::makeChoice, this, true));
    no->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&MessagePrinter::makeChoice, this, false));
    keyboard.setPosition({TextPos.x, TextPos.y - keyboard.getSize().y - 12.f});

    menu.addToScene(scene);
    triangle.addToScene(scene, bl::rc::UpdateSpeed::Static);
    text.addToScene(scene, bl::rc::UpdateSpeed::Static);
}

void MessagePrinter::setMessage(BattleState& state, const Message& msg) {
    std::string dispText;

    const bool forPlayer     = msg.forHostBattler() == state.localPlayer().isHost();
    Battler& battler         = forPlayer ? state.localPlayer() : state.enemy();
    Battler& otherBattler    = forPlayer ? state.enemy() : state.localPlayer();
    const std::string& ppl   = battler.activePeoplemon().base().name();
    const std::string& other = otherBattler.activePeoplemon().base().name();
    bool clear               = true;

    setState(State::Printing);

    switch (msg.getType()) {
    case Message::Type::Attack:
        dispText = state.activeBattler().activePeoplemon().base().name() + " used " +
                   core::pplmn::Move::name(msg.getMoveId()) + "!";
        break;

    case Message::Type::Callback:
        if (forPlayer) { dispText = "Come back " + ppl + "!"; }
        else { dispText = state.enemy().name() + " called back " + ppl + "!"; }
        break;

    case Message::Type::SendOut:
        if (forPlayer) { dispText = "Go " + ppl + "!"; }
        else { dispText = state.enemy().name() + " sent out " + ppl + "!"; }
        break;

    case Message::Type::SuperEffective:
        dispText = "It was super effective!";
        break;

    case Message::Type::NotEffective:
        dispText = "It was not very effective";
        break;

    case Message::Type::IsNotAffected:
        dispText = other + " is not affected by " +
                   pplmn::Move::name(
                       battler.activePeoplemon().base().knownMoves()[battler.chosenMove()].id);
        break;

    case Message::Type::CriticalHit:
        dispText = "It was a critical hit!";
        break;

    case Message::Type::NetworkIntro:
        dispText = "Your friend " + state.enemy().name() + " wants to fight!";
        break;

    case Message::Type::TrainerIntro:
        dispText = state.enemy().name() + " wants to battle!";
        break;

    case Message::Type::WildIntro:
        dispText = "A wild " + state.enemy().name() + " attacked!";
        break;

    case Message::Type::PlayerFirstSendout:
        dispText = "Go " + state.localPlayer().activePeoplemon().base().name() + "!";
        break;

    case Message::Type::OpponentFirstSendout:
        dispText =
            state.enemy().name() + " used " + state.enemy().activePeoplemon().base().name() + "!";
        break;

    case Message::Type::AttackMissed:
        dispText = "But it missed!";
        break;

    case Message::Type::AttackRestoredHp:
        dispText = ppl + " had it's HP restored!";
        break;

    case Message::Type::GainedAilment:
    case Message::Type::GainedAilmentSnackshare:
        switch (msg.getAilment()) {
        case pplmn::Ailment::Annoyed:
            dispText = ppl + " became Annoyed" + snackShareSuffix(msg.getType(), other);
            break;
        case pplmn::Ailment::Frozen:
            dispText = ppl + " was Frozen" + snackShareSuffix(msg.getType(), other);
            break;
        case pplmn::Ailment::Frustrated:
            dispText = ppl + " became Frustrated" + snackShareSuffix(msg.getType(), other);
            break;
        case pplmn::Ailment::Sleep:
            dispText = ppl + " was afflicted by Sleep" + snackShareSuffix(msg.getType(), other);
            break;
        case pplmn::Ailment::Sticky:
            dispText = ppl + " became Sticky" + snackShareSuffix(msg.getType(), other);
            break;
        case pplmn::Ailment::None:
        default:
            dispText = "<ERROR: Invalid ailment specified in ailment message>";
            break;
        }
        break;

    case Message::Type::AilmentGiveFail:
    case Message::Type::SubstituteAilmentBlocked:
    case Message::Type::GuardBlockedAilment:
        switch (msg.getAilment()) {
        case pplmn::Ailment::Annoyed:
            dispText = other + " tried to Annoy " + ppl + ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::Ailment::Frozen:
            dispText = other + " tried to Freeze " + ppl + ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::Ailment::Frustrated:
            dispText =
                other + " tried to Frustrate " + ppl + ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::Ailment::Sleep:
            dispText = other + " tried to make " + ppl + " fall asleep" +
                       ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::Ailment::Sticky:
            dispText = other + " tried to make " + ppl + " Sticky" +
                       ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::Ailment::None:
        default:
            dispText = "<ERROR: Invalid ailment specified in ailment message>";
            break;
        }
        break;

    case Message::Type::GainedPassiveAilment:
        switch (msg.getPassiveAilment()) {
        case pplmn::PassiveAilment::Confused:
            dispText = ppl + " became Confused!";
            break;
        case pplmn::PassiveAilment::Distracted:
            dispText = ppl + " is now Distracted!";
            break;
        case pplmn::PassiveAilment::Stolen:
            dispText = ppl + " was Jumped!";
            break;
        case pplmn::PassiveAilment::Trapped:
            dispText = ppl + " became Trapped!";
            break;
        case pplmn::PassiveAilment::None:
        default:
            dispText = "<ERROR: Invalid passive ailment specified in ailment message>";
            break;
        }
        break;

    case Message::Type::PassiveAilmentGiveFail:
    case Message::Type::SubstitutePassiveAilmentBlocked:
    case Message::Type::GuardBlockedPassiveAilment:
    case Message::Type::AilmentSatPassiveAilmentBlocked:
        switch (msg.getPassiveAilment()) {
        case pplmn::PassiveAilment::Confused:
            dispText =
                other + " tried to Confuse " + ppl + ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::PassiveAilment::Distracted:
            dispText =
                other + " tried to Distract " + ppl + ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::PassiveAilment::Stolen:
            dispText = other + " tried to Jump " + ppl + ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::PassiveAilment::Trapped:
            dispText = other + " tried to Trap " + ppl + ailmentBlockedSuffix(msg.getType(), ppl);
            break;
        case pplmn::PassiveAilment::None:
        default:
            dispText = "<ERROR: Invalid passive ailment specified in ailment message>";
            break;
        }
        break;

    case Message::Type::GenericMoveFailed:
        dispText = "But it failed!";
        break;

    case Message::Type::WasProtected:
        dispText = "But " + ppl + " Protected itself!";
        break;

    case Message::Type::Guarded:
        dispText = ppl + " became Guarded!";
        break;

    case Message::Type::GuardFailed:
        dispText = ppl + " tried to Guard themselves but they are already Guarded!";
        break;

    case Message::Type::SubstituteSuicide:
        dispText = ppl + " tried to create a Substitute but killed themself instead!";
        break;

    case Message::Type::SubstituteAlreadyExists:
        dispText = ppl + " tried to create a Substitute but they already have one!";
        break;

    case Message::Type::SubstituteCreated:
        dispText = ppl + " created a Substitute!";
        break;

    case Message::Type::SubstituteTookDamage:
        dispText = ppl + "'s Substitute took damage in their place!";
        break;

    case Message::Type::SubstituteDied:
        dispText = ppl + "'s Substitute was killed!";
        break;

    case Message::Type::HealBellHealed:
        dispText = ppl + "'s Heal Bell made their entire party healthy!";
        break;

    case Message::Type::HealBellAlreadyHealthy:
        dispText = ppl + " tried to use their Heal Bell to make their party healthy but everyone "
                         "is already healthy!";
        break;

    case Message::Type::StatIncreased:
        dispText = ppl + "'s " + pplmn::Stats::statToString(msg.getStat()) + " increased!";
        break;

    case Message::Type::StatIncreasedSharply:
        dispText = ppl + "'s " + pplmn::Stats::statToString(msg.getStat()) + " rose sharply!";
        break;

    case Message::Type::StatIncreaseFailed:
        dispText =
            ppl + "'s " + pplmn::Stats::statToString(msg.getStat()) + " cannot go any higher!";
        break;

    case Message::Type::StatDecreased:
        dispText = ppl + "'s " + pplmn::Stats::statToString(msg.getStat()) + " decreased!";
        break;

    case Message::Type::StatDecreasedSharply:
        dispText = ppl + "'s " + pplmn::Stats::statToString(msg.getStat()) + " fell sharply!";
        break;

    case Message::Type::StatDecreaseFailed:
        dispText =
            ppl + "'s " + pplmn::Stats::statToString(msg.getStat()) + " cannot go any lower!";
        break;

    case Message::Type::RecoilDamage:
        dispText = ppl + " was hurt by Recoil!";
        break;

    case Message::Type::ChargeStarted:
        dispText = ppl + " began charging power!";
        break;

    case Message::Type::ChargeUnleashed:
        dispText = ppl + " unleashed power!";
        break;

    case Message::Type::SuicideEffect:
        dispText = ppl + " killed itself!";
        break;

    case Message::Type::CounterEffect:
        dispText = ppl + " was hurt by Counter!";
        break;

    case Message::Type::OnlySleepAffected:
        dispText = "It had no effect because " + other + " is not sleeping!";
        break;

    case Message::Type::PeanutAllergic:
        dispText = ppl + " had an allergic reaction and died!";
        break;

    case Message::Type::PeanutAte:
        dispText = ppl + " ate the peanut and gained HP!";
        break;

    case Message::Type::EveryoneWokenUp:
        dispText = "It is physically impossible for anyone in a 15 mile radius to still be asleep "
                   "after that";
        break;

    case Message::Type::EncoreStart:
        dispText = other + " is trying to make " + ppl + " do an Encore!";
        break;

    case Message::Type::EncoreFailed:
        dispText = ppl + " cannot do an Encore now!";
        break;

    case Message::Type::RandomMove:
        dispText = pplmn::Move::name(msg.getOriginalMove()) + " turned into " +
                   pplmn::Move::name(msg.getNewMove()) + "!";
        break;

    case Message::Type::BatonPassFailed:
        dispText = ppl + " tried to use Baton Pass to switch out but no one is there to help!";
        break;

    case Message::Type::BatonPassStart:
        dispText = ppl + "'s Baton Pass lets them switch out!";
        break;

    case Message::Type::BatonPassStatsCopied:
        dispText = ppl + " received " + battler.peoplemon()[msg.getInt()].base().name() +
                   "'s stats via Baton Pass!";
        break;

    case Message::Type::DeathCountDown:
        dispText = ppl + " has been Marked for Death! They better get out of here!";
        break;

    case Message::Type::DeathFromCountdown:
        dispText = ppl + " died from their Mark of Death! Should have gotten out of here when they "
                         "had the chance.";
        break;

    case Message::Type::DeathCountDownFailed:
        dispText = ppl + " tried to mark themself and " + other +
                   " for death but they are both already doomed!";
        break;

    case Message::Type::GambleOne:
        dispText = ppl + " rolled a 1 and was reduced to 1 HP!";
        break;

    case Message::Type::GambleTwenty:
        dispText = ppl + " rolled a 20!! Attack power is increased to 200!";
        break;

    case Message::Type::GambleMiddle:
        dispText = ppl + " rolled a " + std::to_string(msg.getInt()) + "! Attack power is " +
                   std::to_string(msg.getInt() * 5) + "!";
        break;

    case Message::Type::Absorb:
        dispText = ppl + " absorbed HP from " + other + "!";
        break;

    case Message::Type::NoPPDeath:
        dispText = ppl + " has no PP and died of shame!";
        break;

    case Message::Type::BallServed:
        dispText = ppl + " Served a volleyball out of nowhere! Better watch out.";
        break;

    case Message::Type::BallBumped:
        dispText = ppl + " Bumped the volleyball!";
        break;

    case Message::Type::BallSet:
        dispText = ppl + " Set the volleyball!";
        break;

    case Message::Type::BallSetFail:
        dispText = ppl + " tried to Set a volleyball but there isn't one in play!";
        break;

    case Message::Type::BallSpiked:
        dispText = ppl + " Spiked the volleyball!";
        break;

    case Message::Type::BallSpikeFail:
        dispText = ppl + " tried to Spike a volleyball but there isn't one in play!";
        break;

    case Message::Type::BallSwiped:
        dispText = ppl + " Swiped the volleyball!";
        break;

    case Message::Type::BallSwipeFail:
        dispText = ppl + " tried to Swipe a volleyball but there isn't one in play!";
        break;

    case Message::Type::BallBlocked:
        dispText = ppl + " is preparing to Block a Spike!";
        break;

    case Message::Type::BallBlockFail:
        dispText = ppl + " got ready to Block a Spike, but no one has a ball!";
        break;

    case Message::Type::BallSpikeBlocked:
        dispText = ppl + "'s Spike was Blocked by " + other + " and they died of shame!";
        break;

    case Message::Type::BallSwipeBlocked:
        dispText = other + " did not Block so " + ppl + "'s Swipe didn't work! They died of shame.";
        break;

    case Message::Type::BallKillSelf:
        dispText = ppl + " forgot to keep the volleyball in the air and died of shame!";
        break;

    case Message::Type::BallKillSpike:
        dispText = ppl + " was killed by " + other + "'s Spike!";
        break;

    case Message::Type::BallKillSwipe:
        dispText = ppl + " was killed by " + other + "'s Swipe!";
        break;

    case Message::Type::BallKillTimeout:
        dispText = ppl + " had the volleyball on their side too long and died of shame!";
        break;

    case Message::Type::BallNoSwitchSuicide:
        dispText = ppl + " has no one else on their team to hit the ball and died of shame!";
        break;

    case Message::Type::MaxAtkMinAcc:
        dispText = ppl + "'s ATK was maxed but their ACC is trashed!";
        break;

    case Message::Type::PPLowered:
        dispText = ppl + " lowered " + other + "'s PP";
        break;

    case Message::Type::PPLowerFail:
        dispText = ppl + " tried to lower " + other + "'s PP but they were able to keep it up!";
        break;

    case Message::Type::EndureStart:
        dispText = ppl + " is doing their best to Endure and not die!";
        break;

    case Message::Type::Endured:
        dispText = ppl + " was able to Endure and survive!";
        break;

    case Message::Type::EndureFail:
        dispText = ppl + " tried to Endure but failed and is just as mortal as the rest of us!";
        break;

    case Message::Type::SpikesApplied:
        dispText = other + " threw a bunch of Spikes around " + ppl +
                   "! They better not move around too much.";
        break;

    case Message::Type::SpikesDamage:
        dispText = ppl + " took damage from stepping on the Spikes around them!";
        break;

    case Message::Type::SpikesFailed:
        dispText = other + " tried to throw more Spikes around " + ppl +
                   " but it's too messy so they gave up!";
        break;

    case Message::Type::DoubleBroPower:
        dispText = ppl + "'s Bro Attack Power was increased by " + std::to_string(msg.getInt()) +
                   " Total Bros in their party!";
        break;

    case Message::Type::HealNextStart:
        dispText =
            ppl + " threw some weird powder into the air that will take exactly one turn to fall!";
        break;

    case Message::Type::HealNextHealed:
        dispText = ppl + " was healed by the falling powder!";
        break;

    case Message::Type::HealNextFail:
        dispText = ppl + " tried to throw powder in the air again but the powder already there got "
                         "in their eyes!";
        break;

    case Message::Type::Move64Cancel:
        dispText = ppl + " was unabled to use " + pplmn::Move::name(msg.getMoveId()) +
                   " due to trauma from being Kicked!";
        break;

    case Message::Type::Roar:
        dispText = ppl + " ran away in fear!";
        break;

    case Message::Type::RoarFailedNoSwitch:
        dispText = ppl + " wants to cower in fear but there is no one to take their place!";
        break;

    case Message::Type::RoarClearedArea:
        dispText =
            ppl + "'s Roar was so strong that all the spikes and volleyballs got blown away!";
        break;

    case Message::Type::StatsStolen:
        dispText = ppl + " stole all of " + other + "'s stat changes!";
        break;

    case Message::Type::AttackThenSwitched:
        dispText = ppl + " is able to return to their ball!";
        break;

    case Message::Type::AttackSwitchFailed:
        if (battler.peoplemon().size() > 1) {
            dispText = ppl + " tried to retreat but all of their friends are dead!";
        }
        else { dispText = ppl + " tried to retreat but they don't have any friends!"; }
        break;

    case Message::Type::SleepHealed:
        dispText = ppl + " went to Sleep to try and fix their problems!";
        break;

    case Message::Type::SleepHealFailed:
        dispText = ppl + " tried to Sleep to ignore their problems but stayed wide awake!";
        break;

    case Message::Type::WokeUp:
        dispText = ppl + " woke up!";
        break;

    case Message::Type::DeathSwapSac:
        dispText = ppl + " sacrificed themselves to revive a teammate!";
        break;

    case Message::Type::DeathSwapFailed:
        dispText = ppl + " tried to sacrifice themselves but can't even get that right!";
        break;

    case Message::Type::DeathSwapRevived:
        dispText = ppl + "'s Sacrifice revived " +
                   battler.peoplemon()[battler.chosenPeoplemon()].base().name() + "!";

        break;

    case Message::Type::Fainted:
        dispText = ppl + " \"fainted\" from terrible physical trauma!";
        break;

    case Message::Type::TrainerLost:
        dispText =
            state.enemy().name() + ": " + state.enemy().getSubstate().trainer->loseBattleDialog();
        break;

    case Message::Type::WonMoney:
        dispText = state.localPlayer().name() + " won " + std::to_string(msg.getInt()) + " monies!";
        break;

    case Message::Type::WhiteoutA:
        dispText = state.localPlayer().name() + " is all out of friends!";
        break;

    case Message::Type::WhiteoutB:
        dispText = state.localPlayer().name() + " oranged out!";
        break;

    case Message::Type::NetworkWinLose:
        if (state.localPlayer().canFight()) {
            dispText = "You have beaten your acquaintance " + state.enemy().name() +
                       " at the most advanced fighting game!";
        }
        else { dispText = "A person you know named " + state.enemy().name() + " has shamed you!"; }
        break;

    case Message::Type::AwardedXp:
        dispText = state.localPlayer().peoplemon()[msg.forIndex()].base().name() + " gained " +
                   std::to_string(msg.getUnsigned()) + " XP!";
        break;

    case Message::Type::LevelUp:
        dispText =
            state.localPlayer().peoplemon()[msg.forIndex()].base().name() + " grew to level " +
            std::to_string(state.localPlayer().peoplemon()[msg.forIndex()].base().currentLevel()) +
            "!";
        break;

    case Message::Type::TryingToLearnMove:
        dispText = state.localPlayer().peoplemon()[msg.forIndex()].base().name() +
                   " is trying to learn the move " + pplmn::Move::name(msg.getMoveId()) +
                   " but already knows 4 moves!";
        break;

    case Message::Type::ForgotMove:
        dispText = state.localPlayer().peoplemon()[msg.forIndex()].base().name() + " forgot " +
                   pplmn::Move::name(msg.getMoveId()) + "!";
        break;

    case Message::Type::LearnedMove:
        dispText = state.localPlayer().peoplemon()[msg.forIndex()].base().name() + " learned " +
                   pplmn::Move::name(msg.getMoveId()) + "!";
        break;

    case Message::Type::DidntLearnMove:
        dispText = state.localPlayer().peoplemon()[msg.forIndex()].base().name() +
                   " did not learn " + pplmn::Move::name(msg.getMoveId()) + "!";
        break;

    case Message::Type::AskForgetMove:
        setState(State::PrintingYesNoChoice);
        dispText = "Forget a move to learn " + pplmn::Move::name(msg.getMoveId()) + "?";
        break;

    case Message::Type::IsConfused:
        dispText = ppl + " is Confused!";
        break;

    case Message::Type::HurtConfusion:
        dispText = ppl + " hurt themselves in Confusion!";
        break;

    case Message::Type::SnappedConfusion:
        dispText = ppl + " snapped out of Confusion!";
        break;

    case Message::Type::StolenAilment:
        dispText = ppl + " had it's HP Stolen by " + other + "!";
        break;

    case Message::Type::TrappedAilment:
        dispText = ppl + " was Trapped by " + other + "!";
        break;

    case Message::Type::DistractedAilment:
        dispText = ppl + " was too Distracted to do anything!";
        break;

    case Message::Type::AnnoyAilment:
        dispText = ppl + " was too Annoyed to move!";
        break;

    case Message::Type::FrustratedAilment:
        dispText = ppl + " is physically hurt from pure Frustration!";
        break;

    case Message::Type::StickyAilment:
        dispText = ppl + " is getting really hurt from being so Sticky!";
        break;

    case Message::Type::SleepingAilment:
        dispText = ppl + " is Sleeping on the job!";
        break;

    case Message::Type::FrozenAilment:
        dispText = ppl + " is Frozen and cannot move!";
        break;

    case Message::Type::ThawedOut:
        dispText = ppl + " thawed out and is no longer Frozen!";
        break;

    case Message::Type::BoardGameSwitchBlocked:
        dispText = other + "'s Board Game Master abilities prevents " + ppl + " from leaving!";
        break;

    case Message::Type::ChillaxCritBlocked:
        dispText = ppl + " is too Chillaxed to receive critical hits!";
        break;

    case Message::Type::ClassyFrustratedBlocked:
        dispText = other + " tried to Frustrate " + ppl + " but " + ppl +
                   " is too Classy to be Frustrated so easily!";
        break;

    case Message::Type::GoonDamage:
        dispText = ppl + " got roughed up a bit because " + other + " is a Goon!";
        break;

    case Message::Type::QuickDrawFirst:
        dispText = ppl + "'s Quick Draw lets them move first!";
        break;

    case Message::Type::FriendlyAilmentHeal:
        dispText = battler.peoplemon()[msg.getInt()].base().name() +
                   " had their ailment healed because they are Always Friendly!";
        break;

    case Message::Type::SassyDamage:
        dispText = ppl + " took some damage from " + other + "'s Sassiness!";
        break;

    case Message::Type::BeefyPower:
        dispText = ppl + " Beefed Up their Athletic attack out of desparation!";
        break;

    case Message::Type::ReservedQuietPower:
        dispText = ppl + "'s Quiet move is powered up because they are Reserved and almost dead!";
        break;

    case Message::Type::DukeOfJokes:
        dispText = ppl + " powers up Jokes because they are the Duke of Jokes!";
        break;

    case Message::Type::EngagingAbility:
        dispText = pplmn::Move::name(msg.getMoveId()) + " normally doesn't affect " + other +
                   " but " + ppl + " is so Engaging that it's not very effective instead!";
        break;

    case Message::Type::DerpDerpConfuse:
        dispText = ppl + " was Confused by " + other + "'s Derpiness!";
        break;

    case Message::Type::KlutzDrop:
        dispText = ppl + " dropped their " + item::Item::getName(msg.getItem()) +
                   " because they are a Klutz!";
        break;

    case Message::Type::SidetrackDistract:
        dispText = ppl + " went on a Sidetrack and Distracted " + other + "!";
        break;

    case Message::Type::NoJokeTeachAbility:
        dispText = ppl + "'s Jokes will not work on " + other + " while they are Teaching!";
        break;

    case Message::Type::FieryTeachAbility:
        dispText = ppl + " got Fired Up from Teaching!";
        break;

    case Message::Type::ExperiencedTeachAbility:
        dispText = ppl + " is too much of an Experienced Teacher to die while Teaching!";
        break;

    case Message::Type::NewTeachAbility:
        dispText = ppl + " is slightly faster this turn because they are a New Teacher!";
        break;

    case Message::Type::DozeOffAbility:
        dispText = ppl + "'s Teaching made " + other + " Doze Off!";
        break;

    case Message::Type::DouseFlamesAbility:
        dispText = other + " Doused the Flames of " + ppl + "'s attack!";
        break;

    case Message::Type::FlirtyAbility:
        dispText = other + "'s Flirtyness is disarming " + ppl + "!";
        break;

    case Message::Type::UndyingFaithAbility:
        dispText = ppl + "'s Undying Faith saved them from death!";
        break;

    case Message::Type::TooCoolAbility:
        dispText = other + " tried to Annoy " + ppl + " but " + ppl + " is Too Cool for that!";
        break;

    case Message::Type::FakeStudyAbility:
        dispText = ppl + " was too busy Fake Studying to attack!";
        break;

    case Message::Type::AlcoholicAbility:
        dispText = ppl + " drank some alcohol from the bag because they are an Alcoholic!";
        break;

    case Message::Type::AlcoholicAbilityFailed:
        dispText = ppl + " tried to get some alcohol from the bag but there isn't any!";
        break;

    case Message::Type::TotalMomAbility: {
        const std::string& mom = battler.peoplemon()[msg.getInt()].base().name();
        dispText = ppl + " had some PP restored by " + mom + " because " + mom + " is a Total Mom!";
    } break;

    case Message::Type::CantSwimAbility:
        dispText = ppl + "'s Accuracy is reduced because " + other + " Can't Swim!";
        break;

    case Message::Type::AllNighterAbility:
        dispText = ppl + " has pulled too many All Nighters to Sleep now!";
        break;

    case Message::Type::AdamentAbility:
        dispText =
            other + " tried to get " + ppl + " to leave, but " + ppl + " is Adament on staying!";
        break;

    case Message::Type::AbsPitchNotEffective:
        dispText = ppl + "'s Absolute Pitch weakens ineffective moves even more!";
        break;

    case Message::Type::AbsPitchSuperEffective:
        dispText = ppl + "'s Absolute Pitch strengthens effective moves even more!";
        break;

    case Message::Type::GameMakerVirusAbility:
        dispText = ppl + " had it's PP lowered by " + other + "'s GameMaker Virus!";
        break;

    case Message::Type::SnapshotAbility:
        dispText =
            ppl + "'s Snapshot prevents the same move from being super effective twice in a row!";
        break;

    case Message::Type::GetBakedAbility:
        dispText = ppl + " is Getting Baked instead of listening!";
        break;

    case Message::Type::PreUseItem:
        dispText = battler.name() + " used a " + item::Item::getName(msg.getItem()) + "!";
        break;

    case Message::Type::ItemUseResult:
        dispText = item::Item::getUseLine(msg.getItem(),
                                          state.activeBattler().peoplemon()[msg.forIndex()].base());
        break;

    case Message::Type::ItemNoEffect:
        dispText = "It had no effect! What a waste of time.";
        break;

    case Message::Type::BagOfGoldfish:
        dispText = ppl + "'s ate from their Bag of Goldfish and regained HP!";
        break;

    case Message::Type::BackwardsHoodyStatDown:
        dispText = ppl + "'s Backwards Hoodie makes their attacks more powerful!";
        break;

    case Message::Type::BackwordsHoodyConfuse:
        dispText = ppl + " was Confused by " + other + "'s Backwards Hoodie!";
        break;

    case Message::Type::GlassesAcc:
        dispText = ppl + "'s Glasses increase ACC!";
        break;

    case Message::Type::SlappingGloveDamage:
        dispText = ppl + "'s ATK is increased by their Slapping Glove!";
        break;

    case Message::Type::SpoonDamage:
        dispText = ppl + "'s SPATK is increased by their Spoon!";
        break;

    case Message::Type::SuperTinyMiniFridge:
        dispText = ppl + "'s Super Tiny Mini Fridge exploded!";
        break;

    case Message::Type::SketchySack:
        dispText =
            ppl + " took damage from their Sketchy Sack that they are holding for some reason!";
        break;

    case Message::Type::GoldfishCracker:
        dispText = ppl + " ate their abnormally large Goldfish Cracker and regained HP!";
        break;

    case Message::Type::WakeUpBelle:
        dispText = ppl + " was woken up by their Wakeup Belle!";
        break;

    case Message::Type::PowerJuice:
        dispText = ppl + " drank their Power Juice and increased attack power!";
        break;

    case Message::Type::IcedTea:
        dispText = ppl + " drank their Iced Tea and increased defense!";
        break;

    case Message::Type::SpeedJuice:
        dispText = ppl + " drank their Speed Juice and increased their speed!";
        break;

    case Message::Type::RunFailedNotWild:
        dispText = "You can't run from this fight, coward!";
        break;

    case Message::Type::RunFailed:
        dispText = "Tried to run away but " + ppl + " was too slow and now it's awkward!";
        break;

    case Message::Type::RunAway:
        dispText = "Got away safely!";
        break;

    case Message::Type::PeopleballNoSteal:
        dispText = "You can't catch other people's Peoplemon! It's unethical!";
        break;

    case Message::Type::PeopleballBrokeout:
        switch (bl::util::Random::get<int>(0, 3)) {
        case 0:
            dispText = ppl + " broke out!";
            break;
        case 1:
            dispText = ppl + " is too squirmy!";
            break;
        case 2:
            dispText = ppl + " doesn't want to go out with you!";
            break;
        case 3:
            dispText = ppl + " is trying to get away and contact the police!";
            break;
        }
        break;

    case Message::Type::PeopleballCaught:
        dispText = ppl + " was caught!";
        break;

    case Message::Type::AskToSetNickname:
        setState(State::PrintingYesNoChoice);
        dispText = "Give " + ppl + " a nickname?";
        break;

    case Message::Type::AskForNickname:
        setState(State::WaitingNameEntry);
        dispText = "What should " + ppl + " be called?";
        keyboard.start(0, 32);
        clear = false;
        break;

    case Message::Type::SentToStorage:
        dispText = (keyboard.value().empty() ? ppl : keyboard.value()) + " was sent to Storage.";
        break;

    case Message::Type::StorageFailed:
        dispText = "Your Peoplemon storage is full! " +
                   (keyboard.value().empty() ? ppl : keyboard.value()) +
                   " was released to avoid a wrongful death lawsuit.";
        break;

    case Message::Type::PeoplemonCloned:
        dispText = ppl + " was cloned! Holy shit!";
        break;

    case Message::Type::CloneFailed:
        dispText = ppl + " could not be cloned!";
        break;

    case Message::Type::_ERROR:
        break;
    }

    if (dispText.empty()) {
        BL_LOG_WARN << "Got bad message type: " << msg.getType();
        dispText = "<BAD MESSAGE TYPE>";
    }

    text.getSection().setString(dispText);
    writer.setContent(text.getSection().getString().toAnsiString());
    if (clear) text.getSection().setString("");
}

void MessagePrinter::process(input::EntityControl ctrl, bool fromEvent) {
    switch (state) {
    case State::Printing:
    case State::PrintingYesNoChoice:
    case State::ShowingNotAcked:
        if ((ctrl == input::Control::Back || ctrl == input::Control::Interact) && fromEvent) {
            finishPrint();
        }
        break;
    case State::WaitingYesNoChoice:
        inputDriver.sendControl(ctrl, fromEvent);
        break;
    case State::WaitingNameEntry:
        keyboard.process(ctrl, fromEvent);
        break;
    default:
        break;
    }
}

void MessagePrinter::finishPrint() {
    switch (state) {
    case State::PrintingYesNoChoice:
        setState(State::WaitingYesNoChoice);
        [[fallthrough]];

    case State::Printing:
        writer.showAll();
        text.getSection().setString(std::string(writer.getVisible()));
        if (state == State::Printing) { setState(State::ShowingNotAcked); }
        break;

    case State::ShowingNotAcked:
        setState(State::ShowingAcked);
        break;

    default:
        break;
    }
}

void MessagePrinter::hide() { setState(State::Hidden); }

bool MessagePrinter::messageDone() const {
    return state == State::ShowingAcked || state == State::Hidden;
}

void MessagePrinter::update(float dt) {
    switch (state) {
    case State::PrintingYesNoChoice:
    case State::Printing: {
        const std::size_t oldLen = writer.getVisible().size();
        writer.update(dt);
        if (oldLen != writer.getVisible().size()) {
            text.getSection().setString(std::string(writer.getVisible()));
        }
        if (writer.finished()) {
            if (state == State::Printing) { setState(State::ShowingNotAcked); }
            else { setState(State::WaitingYesNoChoice); }
        }
    } break;

    default:
        break;
    }
}

void MessagePrinter::render(bl::rc::scene::CodeScene::RenderContext& ctx) {
    if (state != State::Hidden) {
        text.draw(ctx);

        switch (state) {
        case State::ShowingNotAcked:
            triangle.draw(ctx);
            break;
        case State::WaitingYesNoChoice:
            menu.draw(ctx);
            break;
        default:
            break;
        }
    }
}

void MessagePrinter::makeChoice(bool f) {
    choseYes = f;
    setState(State::Hidden);
}

bool MessagePrinter::choseToForget() const { return choseYes; }

bool MessagePrinter::choseToSetName() const { return choseYes; }

void MessagePrinter::nameEntered() { setState(State::Hidden); }

const std::string& MessagePrinter::chosenNickname() const { return keyboard.value(); }

void MessagePrinter::setState(State ns) {
    if (state == State::WaitingNameEntry) { keyboard.stop(); }

    state = ns;
    triangle.resetFlash();

    if (ns == State::WaitingNameEntry) { keyboard.start(0, 32); }
}

} // namespace view
} // namespace battle
} // namespace core
