#include <Core/Systems/HUD/QtyEntry.hpp>

#include <Core/Properties.hpp>

namespace core
{
namespace system
{
namespace hud
{
QtyEntry::QtyEntry()
: upArrow({6.f, 0.f}, {12.f, 8.f}, {0.f, 8.f})
, downArrow({0.f, 0.f}, {12.f, 0.f}, {6.f, 8.f})
, qty(0)
, minQty(0)
, maxQty(0) {
    background.setFillColor(sf::Color::White);
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(2.f);

    text.setFillColor(sf::Color::Black);
    text.setFont(Properties::MenuFont());
    text.setCharacterSize(14);

    upArrow.setFillColor(sf::Color::Black);
    downArrow.setFillColor(sf::Color::Black);
}

void QtyEntry::setPosition(const sf::Vector2f& pos) { position = pos; }

sf::Vector2f QtyEntry::getSize() const {
    return {background.getGlobalBounds().width, background.getGlobalBounds().height};
}

void QtyEntry::configure(int mn, int mx, int q) {
    // determine max text width
    text.setString(std::to_string(mn));
    float w = text.getGlobalBounds().width;
    text.setString(std::to_string(mx));
    w = std::max(w, text.getGlobalBounds().width);

    // update positions
    background.setSize({w + 12.f,
                        upArrow.getGlobalBounds().height + downArrow.getGlobalBounds().height +
                            text.getGlobalBounds().height + 6.f + text.getLocalBounds().top + 6.f});
    upArrow.setPosition(w * 0.5f + 6.f, 7.f);
    downArrow.setPosition(w * 0.5f + 6.f,
                          background.getSize().y - downArrow.getGlobalBounds().height - 1.f);
    text.setPosition(0.f, upArrow.getGlobalBounds().height + 5.f);

    // update data
    qty    = q;
    minQty = mn;
    maxQty = mx;
    updateText();
}

void QtyEntry::updateText() {
    text.setString(std::to_string(qty));
    text.setPosition(background.getSize().x * 0.5f - text.getGlobalBounds().width * 0.5f,
                     text.getPosition().y);
}

int QtyEntry::curQty() const { return qty; }

void QtyEntry::up(int q) {
    if (rateLimit()) return;
    qty = std::min(maxQty, qty + q);
    updateText();
}

void QtyEntry::down(int q) {
    if (rateLimit()) return;
    qty = std::max(minQty, qty - q);
    updateText();
}

void QtyEntry::render(sf::RenderTarget& target) const {
    sf::RenderStates states;
    states.transform.translate(position);

    target.draw(background, states);
    target.draw(text, states);
    if (qty > minQty) { target.draw(downArrow, states); }
    if (qty < maxQty) { target.draw(upArrow, states); }
}

bool QtyEntry::rateLimit() {
    // TODO - we may want to differentiate between held and repeatedly pressed inputs
    if (debounce.getElapsedTime().asSeconds() >= 0.4f) {
        debounce.restart();
        return false;
    }
    return true;
}

} // namespace hud
} // namespace system
} // namespace core
