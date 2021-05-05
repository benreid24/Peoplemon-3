#ifndef CORE_PLAYER_INPUT_MENUDRIVER_HPP
#define CORE_PLAYER_INPUT_MENUDRIVER_HPP

#include <BLIB/Menu.hpp>
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
     * @brief Forwards the corresponding menu event to the menu being driven, if set
     *
     * @param command The command to handle
     */
    virtual void process(component::Command command) override;

private:
    bl::menu::Menu* menu;
    bool back;
};

} // namespace input
} // namespace player
} // namespace core

#endif
