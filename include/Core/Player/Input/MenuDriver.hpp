#ifndef CORE_PLAYER_INPUT_MENUDRIVER_HPP
#define CORE_PLAYER_INPUT_MENUDRIVER_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Player/Input/Listener.hpp>

namespace core
{
namespace player
{
namespace input
{
/**
 * @brief Helper class that implements input Listener and can drive menus
 *
 * @ingroup Player
 *
 */
class MenuDriver : public Listener {
public:
    /**
     * @brief Construct a new Menu Driver object
     *
     */
    MenuDriver();

    /**
     * @brief Destroy the Menu Driver object
     *
     */
    virtual ~MenuDriver() = default;

    /**
     * @brief Sets the menu to control
     *
     * @param menu The menu to control
     */
    void drive(bl::menu::Menu& menu);

    /**
     * @brief Returns true if Back was pressed. Not processed by Menu, must be done manually
     *
     */
    bool backPressed();

    /**
     * @brief Returns true if Pause was pressed. Not processed by Menu
     *
     */
    bool pausePressed();

    /**
     * @brief Forwards the corresponding menu event to the menu being driven, if set
     *
     * @param command The command to handle
     */
    virtual void process(component::Command command) override;

    /**
     * @brief Sets the minimum time, in seconds, between input events being processed. This limits
     *        the speed of cursor movement to something reasonable. Default is 0.3s
     *
     * @param delay Time between input events in seconds
     */
    void setInputDelay(float delay);

private:
    bl::menu::Menu* menu;
    bool back;
    bool pause;
    float debounce;
    float lastInput;
};

} // namespace input
} // namespace player
} // namespace core

#endif
