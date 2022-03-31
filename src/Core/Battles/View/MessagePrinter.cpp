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
} // namespace

MessagePrinter::MessagePrinter()
: triangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flasher(triangle, 0.75f, 0.65f)
, acked(false) {
    text.setFillColor(sf::Color::Black);
    text.setFont(Properties::MenuFont());
    text.setCharacterSize(13.f);
    text.setPosition(TextPos);
}

void MessagePrinter::setMessage(BattleState&, const Message& msg) {
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
    flasher.render(target, {}, 0.f);
}

} // namespace view
} // namespace battle
} // namespace core