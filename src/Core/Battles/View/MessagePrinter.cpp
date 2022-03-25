#include <Core/Battles/View/MessagePrinter.hpp>

namespace core
{
namespace battle
{
namespace view
{
MessagePrinter::MessagePrinter()
: triangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flasher(triangle, 0.75f, 0.65f)
, acked(false) {
    // TODO - init text
}

void MessagePrinter::setMessage(const std::string&) {
    // TODO
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