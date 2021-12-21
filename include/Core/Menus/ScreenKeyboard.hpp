#ifndef CORE_MENUS_SCREENKEYBOARD_HPP
#define CORE_MENUS_SCREENKEYBOARD_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Player/Input/MenuDriver.hpp>
#include <SFML/Graphics.hpp>

/**
 * @addtogroup Menus
 * @ingroup Core
 * @brief Collection of shared menu components used in the game
 *
 */

namespace core
{
/// Collection of shared menu components used in the game
namespace menu
{
/**
 * @brief A screen keyboard for getting arbitrary user input
 *
 * @ingroup Menus
 *
 */
class ScreenKeyboard
: public bl::event::Listener<sf::Event>
, public sf::Drawable {
public:
    using OnSubmit = std::function<void(const std::string&)>;

    /**
     * @brief Construct a new Screen Keyboard
     *
     * @param bus The event bus to subscribe to when start() is called
     * @param onSubmit Callback to call when the input is submitted
     *
     */
    ScreenKeyboard(bl::event::Dispatcher& bus, const OnSubmit& onSubmit);

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
     * @brief Handles the given window event
     *
     */
    virtual void observe(const sf::Event& event) override;

private:
    const OnSubmit onSubmit;
    unsigned int minLen;
    unsigned int maxLen;
    bl::event::Dispatcher& bus;
    std::string input;
    bool keyboardEnabled;

    sf::Vector2f position;
    bl::resource::Resource<sf::Texture>::Ref bgndTxtr;
    sf::Sprite background;
    sf::Text renderedInput;

    bl::menu::ArrowSelector::Ptr selector;
    core::player::input::MenuDriver inputDriver;
    bl::resource::Resource<sf::Texture>::Ref enableKbTxtr;
    bl::resource::Resource<sf::Texture>::Ref disableKbTxtr;
    bl::menu::ImageItem::Ptr kbSwitch;
    bl::menu::Menu keyboardMenu;

    void onKeyPress(char key);
    void onEnter();
    void onKeyboardToggle();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // namespace menu
} // namespace core

#endif
