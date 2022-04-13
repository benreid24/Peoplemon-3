#include <Core/Battles/View/MessagePrinter.hpp>

#include <BLIB/Interfaces/Utilities/WordWrap.hpp>
#include <Core/Battles/BattleState.hpp>
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
} // namespace

MessagePrinter::MessagePrinter()
: triangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flasher(triangle, 0.75f, 0.65f)
, acked(false) {
    text.setFillColor(sf::Color::Black);
    text.setFont(Properties::MenuFont());
    text.setCharacterSize(18.f);
    text.setPosition(TextPos);
    triangle.setPosition(ArrowPos);
    triangle.setFillColor(sf::Color(242, 186, 17));
}

void MessagePrinter::setMessage(BattleState& state, const Message& msg) {
    std::string dispText;

    switch (msg.getType()) {
    case Message::Type::Attack:
        // TODO - generate actual message
        dispText = "An attack was used";
        break;

    case Message::Type::Callback:
        // TODO - generate actual message
        dispText = "A peoplemon is being called back";
        break;

    case Message::Type::SendOut:
        // TODO - generate actual message
        dispText = "A peoplemon is being sent out";
        break;

    case Message::Type::SuperEffective:
        // TODO - generate actual message
        dispText = "A move was super effective";
        break;

    case Message::Type::NetworkIntro:
        dispText = "Your friend " + msg.getString() + " wants to fight!";
        break;

    case Message::Type::TrainerIntro:
        dispText = msg.getString() + " wants to battle!";
        break;

    case Message::Type::WildIntro:
        dispText = "A wild " + msg.getString() + " attacked!";
        break;

    case Message::Type::PlayerFirstSendout:
        dispText = "Go " + state.localPlayer().activePeoplemon().base().name() + "!";
        break;

    case Message::Type::OpponentFirstSendout:
        dispText =
            state.enemy().name() + " used " + state.enemy().activePeoplemon().base().name() + "!";
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
}

void MessagePrinter::finishPrint() {
    if (!writer.finished()) { writer.showAll(); }
    else {
        acked = true;
        text.setString("");
    }
}

bool MessagePrinter::messageDone() const { return acked; }

void MessagePrinter::update(float dt) {
    const std::size_t oldLen = writer.getVisible().size();

    writer.update(dt);
    flasher.update(dt);

    if (oldLen != writer.getVisible().size()) { text.setString(std::string(writer.getVisible())); }
}

void MessagePrinter::render(sf::RenderTarget& target) const {
    target.draw(text);
    if (writer.finished() && !acked) {
        sf::RenderStates states;
        flasher.render(target, {}, 0.f);
    }
}

} // namespace view
} // namespace battle
} // namespace core
