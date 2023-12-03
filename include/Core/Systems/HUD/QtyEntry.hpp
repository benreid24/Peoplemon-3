#ifndef CORE_SYSTEMS_HUD_QTYENTRY_HPP
#define CORE_SYSTEMS_HUD_QTYENTRY_HPP

#include <BLIB/Graphics/Triangle.hpp>

namespace core
{
namespace system
{
namespace hud
{
/**
 * @brief Helper class to render a number selector
 *
 * @ingroup HUD
 *
 */
class QtyEntry {
public:
    /**
     * @brief Construct a new quantity entry helper
     *
     */
    QtyEntry();

    /**
     * @brief Sets the position of the selector
     *
     * @param position The position to set
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief Returns the size of the entry in pixels
     *
     */
    sf::Vector2f getSize() const;

    /**
     * @brief Configures the entry with the given parameters
     *
     * @param minQty The minimum number to allow entering
     * @param maxQty The maximum number to allow entering
     * @param qty The number to start with
     */
    void configure(int minQty, int maxQty, int qty);

    /**
     * @brief Returns the currently selected qty
     *
     */
    int curQty() const;

    /**
     * @brief Increments the qty up
     *
     * @param q The amount to go up by
     * @param ignoreDebounce True to always modify, false to rate limit
     *
     */
    void up(int q, bool ignoreDebounce);

    /**
     * @brief Decrements the qty down
     *
     * @param q The amount to go down by
     *  @param ignoreDebounce True to always modify, false to rate limit
     *
     */
    void down(int q, bool ignoreDebounce);

    /**
     * @brief Renders the selector
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

private:
    // TODO - BLIB_UPGRADE - update qty entry rendering

    sf::Vector2f position;
    sf::RectangleShape background;
    sf::Text text;
    /*bl::shapes::Triangle upArrow;
    bl::shapes::Triangle downArrow;*/
    sf::Clock debounce;

    int qty;
    int minQty;
    int maxQty;

    void updateText();
    bool rateLimit(bool ignore);
};
} // namespace hud
} // namespace system
} // namespace core

#endif
