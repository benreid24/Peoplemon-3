#ifndef CORE_SYSTEMS_HUD_QTYENTRY_HPP
#define CORE_SYSTEMS_HUD_QTYENTRY_HPP

#include <BLIB/Graphics.hpp>

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
     * @param engine The game engine instance
     */
    QtyEntry(bl::engine::Engine& engine);

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
     * @brief Configures the entry with the given parameters and adds to the overlay
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
     * @brief Removes the quantity entry from the overlay
     */
    void hide();

private:
    bl::engine::Engine& engine;
    bl::rc::Overlay* currentOverlay;
    sf::Vector2f position;
    bl::gfx::Rectangle background;
    bl::gfx::Text text;
    bl::gfx::Triangle upArrow;
    bl::gfx::Triangle downArrow;
    sf::Clock debounce;

    int qty;
    int minQty;
    int maxQty;

    void ensureCreated();
    void updateText();
    bool rateLimit(bool ignore);
};
} // namespace hud
} // namespace system
} // namespace core

#endif
