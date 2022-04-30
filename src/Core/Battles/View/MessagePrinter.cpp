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
: triangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flasher(triangle, 0.75f, 0.65f)
, acked(false)
, visible(false) {
    text.setFillColor(sf::Color::Black);
    text.setFont(Properties::MenuFont());
    text.setCharacterSize(18.f);
    text.setPosition(TextPos);
    triangle.setPosition(ArrowPos);
    triangle.setFillColor(sf::Color(242, 186, 17));
}

void MessagePrinter::setMessage(BattleState& state, const Message& msg) {
    std::string dispText;
    const bool forPlayer =
        msg.forActiveBattler() == (&state.activeBattler() == &state.localPlayer());
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

    default:
        BL_LOG_WARN << "Got bad message type: " << msg.getType();
        dispText = "<BAD MESSAGE TYPE>";
        break;
    }

    text.setString(dispText);
    bl::interface::wordWrap(text, TextWidth);
    writer.setContent(text.getString().toAnsiString());
    text.setString("");
    acked   = false;
    visible = true;
}

void MessagePrinter::finishPrint() {
    if (!writer.finished()) {
        writer.showAll();
        text.setString(std::string(writer.getVisible()));
    }
    else {
        acked = true;
    }
}

void MessagePrinter::hide() { visible = false; }

bool MessagePrinter::messageDone() const { return acked; }

void MessagePrinter::update(float dt) {
    const std::size_t oldLen = writer.getVisible().size();

    writer.update(dt);
    flasher.update(dt);

    if (oldLen != writer.getVisible().size()) { text.setString(std::string(writer.getVisible())); }
}

void MessagePrinter::render(sf::RenderTarget& target) const {
    if (visible) {
        target.draw(text);
        if (writer.finished() && !acked) {
            sf::RenderStates states;
            flasher.render(target, {}, 0.f);
        }
    }
}

} // namespace view
} // namespace battle
} // namespace core
