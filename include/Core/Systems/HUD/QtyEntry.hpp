#ifndef CORE_SYSTEMS_HUD_QTYENTRY_HPP
#define CORE_SYSTEMS_HUD_QTYENTRY_HPP

#include <BLIB/Media/Shapes/Triangle.hpp>
#include <SFML/Graphics.hpp>

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
     *
     */
    void up(int q = 1);

    /**
     * @brief Decrements the qty down
     *
     * @param q The amount to go down by
     *
     */
    void down(int q = 1);

    /**
     * @brief Renders the selector
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

private:
    sf::Vector2f position;
    sf::RectangleShape background;
    sf::Text text;
    bl::shapes::Triangle upArrow;
    bl::shapes::Triangle downArrow;
    sf::Clock debounce;

    int qty;
    int minQty;
    int maxQty;

    void updateText();
    bool rateLimit();
};
} // namespace hud
} // namespace system
} // namespace core

#endif
