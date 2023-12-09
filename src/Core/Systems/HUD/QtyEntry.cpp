#include <Core/Systems/HUD/QtyEntry.hpp>

#include <Core/Properties.hpp>

namespace core
{
namespace system
{
namespace hud
{
namespace
{
constexpr float ArrowHeight = 8.f;
}

QtyEntry::QtyEntry(bl::engine::Engine& engine)
: engine(engine)
, currentOverlay(nullptr)
, qty(0)
, minQty(0)
, maxQty(0) {}

void QtyEntry::ensureCreated() {
    if (background.entity() == bl::ecs::InvalidEntity) {
        background.create(engine, {10.f, 10.f});
        background.setFillColor(sf::Color::White);
        background.setOutlineColor(sf::Color::Black);
        background.setOutlineThickness(2.f);

        text.create(engine, Properties::MenuFont(), "0", 14, sf::Color::Black);
        text.setParent(background);

        upArrow.create(engine, {6.f, 0.f}, {12.f, ArrowHeight}, {0.f, ArrowHeight});
        upArrow.setFillColor(sf::Color::Black);
        upArrow.setParent(background);

        downArrow.create(engine, {0.f, 0.f}, {12.f, 0.f}, {6.f, ArrowHeight});
        downArrow.setFillColor(sf::Color::Black);
        downArrow.setParent(background);
    }

    if (!currentOverlay ||
        engine.renderer().getObserver().getOrCreateSceneOverlay() != currentOverlay) {
        currentOverlay = engine.renderer().getObserver().getOrCreateSceneOverlay();
        background.addToScene(currentOverlay, bl::rc::UpdateSpeed::Static);
        text.addToScene(currentOverlay, bl::rc::UpdateSpeed::Static);
        upArrow.addToScene(currentOverlay, bl::rc::UpdateSpeed::Static);
        downArrow.addToScene(currentOverlay, bl::rc::UpdateSpeed::Static);
    }
}

void QtyEntry::hide() {
    if (background.entity() != bl::ecs::InvalidEntity) { background.removeFromScene(); }
}

void QtyEntry::setPosition(const sf::Vector2f& pos) { position = pos; }

sf::Vector2f QtyEntry::getSize() const { return {background.getSize().x, background.getSize().y}; }

void QtyEntry::configure(int mn, int mx, int q) {
    ensureCreated();

    // determine max text width
    text.getSection().setString(std::to_string(mn));
    float w = text.getLocalBounds().width;
    text.getSection().setString(std::to_string(mx));
    w = std::max(w, text.getLocalBounds().width);

    // update positions
    background.setSize(
        {w + 12.f,
         ArrowHeight * 2.f + text.getLocalBounds().height + 6.f + text.getLocalBounds().top + 6.f});
    upArrow.getTransform().setPosition(w * 0.5f + 6.f, 7.f);
    downArrow.getTransform().setPosition(w * 0.5f + 6.f,
                                         background.getSize().y - ArrowHeight - 1.f);
    text.getTransform().setPosition(0.f, ArrowHeight + 5.f);

    // update data
    qty    = q;
    minQty = mn;
    maxQty = mx;
    updateText();
}

void QtyEntry::updateText() {
    text.getSection().setString(std::to_string(qty));
    text.getTransform().setPosition(background.getSize().x * 0.5f -
                                        text.getLocalBounds().width * 0.5f,
                                    text.getTransform().getLocalPosition().y);
    upArrow.setHidden(qty >= maxQty);
    downArrow.setHidden(qty <= minQty);
}

int QtyEntry::curQty() const { return qty; }

void QtyEntry::up(int q, bool ignore) {
    if (rateLimit(ignore)) return;
    qty = std::min(maxQty, qty + q);
    bl::audio::AudioSystem::playOrRestartSound(Properties::MenuMoveSound());
    updateText();
}

void QtyEntry::down(int q, bool ignore) {
    if (rateLimit(ignore)) return;
    qty = std::max(minQty, qty - q);
    bl::audio::AudioSystem::playOrRestartSound(Properties::MenuMoveSound());
    updateText();
}

bool QtyEntry::rateLimit(bool ignore) {
    if (debounce.getElapsedTime().asSeconds() >= 0.4f || ignore) {
        debounce.restart();
        return false;
    }
    return true;
}

} // namespace hud
} // namespace system
} // namespace core
