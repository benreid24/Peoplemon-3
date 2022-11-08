#ifndef CORE_SYSTEMS_HUD_SCREENKEYBOARD_HPP
#define CORE_SYSTEMS_HUD_SCREENKEYBOARD_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <SFML/Graphics.hpp>

/**
 * @addtogroup HUD
 * @ingroup Core
 * @brief Helper classes and utilities for the HUD
 *
 */

namespace core
{
namespace system
{
/// Helper classes and utilities for the HUD
namespace hud
{
/**
 * @brief A screen keyboard for getting arbitrary user input
 *
 * @ingroup HUD
 *
 */
class ScreenKeyboard : public sf::Drawable {
public:
    using OnSubmit = std::function<void(const std::string&)>;

    /**
     * @brief Construct a new Screen Keyboard
     *
     * @param onSubmit Callback to call when the input is submitted
     *
     */
    ScreenKeyboard(const OnSubmit& onSubmit);

    /**
     * @brief Unsubscribes the keyboard if still subscribed
     *
     */
    ~ScreenKeyboard();

    /**
     * @brief Subscribes the keyboard to the event bus
     *
     * @param minLen Minimum input length to accept
     * @param maxLen Maximum input length to accept
     */
    void start(unsigned int minLen = 0, unsigned int maxLen = 16);

    /**
     * @brief Unsubscribes the keyboard from the event bus
     *
     */
    void stop();

    /**
     * @brief Sets the position of the keyboard. The default is (??)
     *
     */
    void setPosition(const sf::Vector2f& position);

    /**
     * @brief Returns the current input value
     *
     */
    const std::string& value() const;

    /**
     * @brief Returns the size of the screen keyboard
     *
     */
    const sf::Vector2f& getSize() const;

    /**
     * @brief Handles player input
     *
     * @param ctrl The control to process, in raw form
     * @param ignoreDebounce True to ignore debounce and process always, false to rate limit
     *
     */
    void process(unsigned int ctrl, bool ignoreDebounce);

private:
    const OnSubmit onSubmit;
    unsigned int minLen;
    unsigned int maxLen;
    std::string input;

    sf::Vector2f position;
    sf::Text renderedInput;
    sf::RectangleShape background;

    core::input::MenuDriver inputDriver;
    bl::menu::Menu keyboardMenu;

    void onKeyPress(char key);
    void onEnter();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // namespace hud
} // namespace system
} // namespace core

#endif
