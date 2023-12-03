#include <Core/Debug/DebugBanner.hpp>

#include <Core/Properties.hpp>

#ifdef PEOPLEMON_DEBUG

namespace core
{
namespace debug
{
namespace
{
const sf::Vector2f ViewSize(Properties::WindowWidth(), Properties::WindowHeight());
}
DebugBanner::DebugBanner()
: view(ViewSize * 0.5f, ViewSize)
, timeout(2.f) {
    // text.setFont(core::Properties::MenuFont());
    text.setFillColor(sf::Color::Cyan);
    text.setOutlineColor(sf::Color::Black);
    text.setCharacterSize(20);
    text.setOutlineThickness(2.f);
}

void DebugBanner::render(sf::RenderTarget& target) {
    if (!get().text.getString().isEmpty() &&
        get().timer.getElapsedTime().asSeconds() <= get().timeout) {
        const sf::View oldView = target.getView();
        target.setView(get().view);
        target.draw(get().text);
        target.setView(oldView);
    }
}

void DebugBanner::display(const std::string& m, float time) {
    sf::Text& t = get().text;
    t.setString(m);
    t.setPosition(sf::Vector2f(
        static_cast<float>(Properties::WindowWidth() - t.getGlobalBounds().width - 10.f), 10.f));
    get().timeout = time;
    get().timer.restart();
}

DebugBanner& DebugBanner::get() {
    static DebugBanner banner;
    return banner;
}

} // namespace debug
} // namespace core

#endif
