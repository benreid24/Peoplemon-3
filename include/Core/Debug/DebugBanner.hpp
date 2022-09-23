#ifndef CORE_DEBUG_DEBUGBANNER_HPP
#define CORE_DEBUG_DEBUGBANNER_HPP

#include <SFML/Graphics.hpp>

#ifdef PEOPLEMON_DEBUG

namespace core
{
namespace debug
{
/**
 * @brief Text banner that can be displayed. Shows text for a few seconds
 *
 * @ingroup Debug
 *
 */
class DebugBanner {
public:
    /**
     * @brief Renders the banner
     *
     * @param target The target to render to
     */
    static void render(sf::RenderTarget& target);

    /**
     * @brief Displays the given message
     *
     * @param message The message to display
     * @param timeout How long to show it for
     */
    static void display(const std::string& message, float timeout = 2.f);

private:
    sf::View view;
    sf::Text text;
    sf::Clock timer;
    float timeout;

    DebugBanner();
    static DebugBanner& get();
};

} // namespace debug
} // namespace core

#endif
#endif
