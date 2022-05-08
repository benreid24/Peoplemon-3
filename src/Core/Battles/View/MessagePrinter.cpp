#include <Core/Battles/View/MessagePrinter.hpp>

#include <BLIB/Interfaces/Utilities/WordWrap.hpp>
#include <Core/Battles/BattleState.hpp>
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
const sf::Vector2f TextPos(12.f, 473.f);
constexpr float TextWidth = 470.f;
const sf::Vector2f ArrowPos(468.f, 573.f);
const sf::Vector2f FlasherPos(476.f, 575.f);

using Message = cmd::Message;

std::string ailmentBlockedSuffix(Message::Type tp) {
    switch (tp) {
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

std::string statString(pplmn::Stat stat) {
    switch (stat) {
    case pplmn::Stat::Accuracy:
        return "ACC";
    case pplmn::Stat::Attack:
        return "ATK";
    case pplmn::Stat::Critical:
        return "CRIT";
    case pplmn::Stat::Defense:
        return "DEF";
    case pplmn::Stat::Evasion:
        return "EVD";
    case pplmn::Stat::HP:
        return "HP";
    case pplmn::Stat::SpecialAttack:
        return "SPATK";
    case pplmn::Stat::SpecialDefense:
        return "SPDEF";
    case pplmn::Stat::Speed:
        return "SPD";
    default:
        return "<ERR>";
    }
}
} // namespace

MessagePrinter::MessagePrinter()
: state(State::Hidden)
, triangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flasher(triangle, 0.75f, 0.65f)
, menu(bl::menu::ArrowSelector::create(8.f, sf::Color::Black)) {
    text.setFillColor(sf::Color::Black);
    text.setFont(Properties::MenuFont());
    text.setCharacterSize(18.f);
    text.setPosition(TextPos);
    triangle.setPosition(ArrowPos);
    triangle.setFillColor(sf::Color(242, 186, 17));

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
}

void MessagePrinter::setMessage(BattleState& state, const Message& msg) {
    std::string dispText;
    const bool forPlayer =
        msg.forActiveBattler() == (&state.activeBattler() == &state.localPlayer());
    Battler& battler = msg.forActiveBattler() ? state.activeBattler() : state.inactiveBattler();
    const std::string& ppl   = msg.forActiveBattler() ?
                                   state.activeBattler().activePeoplemon().base().name() :
                                   state.inactiveBattler().activePeoplemon().base().name();
    const std::string& other = msg.forActiveBattler() ?
                                   state.inactiveBattler().activePeoplemon().base().name() :
                                   state.activeBattler().activePeoplemon().base().name();

    switch (msg.getType()) {
    case Message::Type::Attack:
        dispText = state.activeBattler().activePeoplemon().base().name() + " used " +
                   core::pplmn::Move::name(msg.getMoveId()) + "!";
        break;

    case Message::Type::Callback:
        if (forPlayer) { dispText = "Come back " + ppl + "!"; }
        else {
            dispText = state.enemy().name() + " called back " + ppl + "!";
        }
        break;

    case Message::Type::SendOut:
        if (forPlayer) { dispText = "Go " + ppl + "!"; }
        else {
            dispText = state.enemy().name() + " sent out " + ppl + "!";
        }
        break;

    case Message::Type::SuperEffective:
        dispText = "It was super effective!";
        break;

    case Message::Type::NotEffective:
        dispText = "It was not very effective";
        break;

    case Message::Type::IsNotAffected:
        dispText = state.inactiveBattler().activePeoplemon().base().name() +
                   " is not affected by " +
                   pplmn::Move::name(state.activeBattler()
                                         .activePeoplemon()
                                         .base()
                                         .knownMoves()[state.activeBattler().chosenMove()]
                                         .id);
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
        switch (msg.getAilment()) {
        case pplmn::Ailment::Annoyed:
            dispText = ppl + " became Annoyed!";
            break;
        case pplmn::Ailment::Frozen:
            dispText = ppl + " was Frozen!";
            break;
        case pplmn::Ailment::Frustrated:
            dispText = ppl + " became Frustrated!";
            break;
        case pplmn::Ailment::Sleep:
            dispText = ppl + " was afflicted by Sleep!";
            break;
        case pplmn::Ailment::Sticky:
            dispText = ppl + " became Sticky!";
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
            dispText = other + " tried to Annoy " + ppl + ailmentBlockedSuffix(msg.getType());
            break;
        case pplmn::Ailment::Frozen:
            dispText = other + " tried to Freeze " + ppl + ailmentBlockedSuffix(msg.getType());
            break;
        case pplmn::Ailment::Frustrated:
            dispText = other + " tried to Frustrate " + ppl + ailmentBlockedSuffix(msg.getType());
            break;
        case pplmn::Ailment::Sleep:
            dispText = other + " tried to make " + ppl + " fall asleep," +
                       ailmentBlockedSuffix(msg.getType());
            break;
        case pplmn::Ailment::Sticky:
            dispText =
                other + " tried to make " + ppl + " Sticky," + ailmentBlockedSuffix(msg.getType());
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
        switch (msg.getPassiveAilment()) {
        case pplmn::PassiveAilment::Confused:
            dispText = other + " tried to Confuse " + ppl + ailmentBlockedSuffix(msg.getType());
            break;
        case pplmn::PassiveAilment::Distracted:
            dispText = other + " tried to Distract " + ppl + ailmentBlockedSuffix(msg.getType());
            break;
        case pplmn::PassiveAilment::Stolen:
            dispText = other + " tried to Jump " + ppl + ailmentBlockedSuffix(msg.getType());
            break;
        case pplmn::PassiveAilment::Trapped:
            dispText = other + " tried to Trap " + ppl + ailmentBlockedSuffix(msg.getType());
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
        dispText = ppl + " tried to Guard themselves it failed!";
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
        dispText = ppl + "'s " + statString(msg.getStat()) + " increased!";
        break;

    case Message::Type::StatIncreasedSharply:
        dispText = ppl + "'s " + statString(msg.getStat()) + " rose sharply!";
        break;

    case Message::Type::StatIncreaseFailed:
        dispText = ppl + "'s " + statString(msg.getStat()) + " cannot go any higher!";
        break;

    case Message::Type::StatDecreased:
        dispText = ppl + "'s " + statString(msg.getStat()) + " decreased!";
        break;

    case Message::Type::StatDecreasedSharply:
        dispText = ppl + "'s " + statString(msg.getStat()) + " fell sharply!";
        break;

    case Message::Type::StatDecreaseFailed:
        dispText = ppl + "'s " + statString(msg.getStat()) + " cannot go any lower!";
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
        dispText = ppl + " threw a bunch of Spikes around " + other +
                   "! They better not move around too much.";
        break;

    case Message::Type::SpikesDamage:
        dispText = ppl + " took damage from stepping on the Spikes around them!";
        break;

    case Message::Type::SpikesFailed:
        dispText = ppl + " tried to throw more Spikes around " + other +
                   " but it's too messy so they gave up!";
        break;

    case Message::Type::DoubleBroPower:
        dispText = ppl + "'s Bro Power doubled attack power!";
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
        else {
            dispText = ppl + " tried to retreat but they don't have any friends!";
        }
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
        else {
            dispText = "A person you know named " + state.enemy().name() + " has shamed you!";
        }
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

    default:
        BL_LOG_WARN << "Got bad message type: " << msg.getType();
        dispText = "<BAD MESSAGE TYPE>";
        break;
    }

    text.setString(dispText);
    bl::interface::wordWrap(text, TextWidth);
    writer.setContent(text.getString().toAnsiString());
    text.setString("");
    this->state =
        msg.getType() == Message::Type::AskForgetMove ? State::PrintingMoveChoice : State::Printing;
}

void MessagePrinter::process(component::Command cmd) {
    switch (state) {
    case State::Printing:
    case State::PrintingMoveChoice:
    case State::ShowingNotAcked:
        if (cmd == component::Command::Back || cmd == component::Command::Interact) {
            finishPrint();
        }
        break;
    case State::WaitingMoveChoice:
        inputDriver.process(cmd);
        break;
    default:
        break;
    }
}

void MessagePrinter::finishPrint() {
    switch (state) {
    case State::PrintingMoveChoice:
        state = State::WaitingMoveChoice;
        [[fallthrough]];

    case State::Printing:
        writer.showAll();
        text.setString(std::string(writer.getVisible()));
        if (state == State::Printing) { state = State::ShowingNotAcked; }
        break;

    case State::ShowingNotAcked:
        state = State::ShowingAcked;
        break;

    default:
        break;
    }
}

void MessagePrinter::hide() { state = State::Hidden; }

bool MessagePrinter::messageDone() const {
    return state == State::ShowingAcked || state == State::Hidden;
}

void MessagePrinter::update(float dt) {
    switch (state) {
    case State::PrintingMoveChoice:
    case State::Printing: {
        const std::size_t oldLen = writer.getVisible().size();
        writer.update(dt);
        if (oldLen != writer.getVisible().size()) {
            text.setString(std::string(writer.getVisible()));
        }
        if (writer.finished()) {
            if (state == State::Printing) { state = State::ShowingNotAcked; }
            else {
                state = State::WaitingMoveChoice;
            }
        }
    } break;

    case State::ShowingNotAcked:
        flasher.update(dt);
        break;

    default:
        break;
    }
}

void MessagePrinter::render(sf::RenderTarget& target) const {
    if (state != State::Hidden) {
        target.draw(text);
        if (state == State::ShowingNotAcked) { flasher.render(target, {}, 0.f); }
        if (state == State::WaitingMoveChoice) { menu.render(target); }
    }
}

void MessagePrinter::makeChoice(bool f) {
    forget = f;
    state  = State::Hidden;
}

bool MessagePrinter::choseToForget() const { return forget; }

} // namespace view
} // namespace battle
} // namespace core
