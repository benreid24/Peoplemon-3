#ifndef GAME_STATES_SETTINGSMENU_HPP
#define GAME_STATES_SETTINGSMENU_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
class SettingsMenu
: public State
, public bl::input::Listener {
public:
    /**
     * @brief Creates the settings menu state
     *
     * @param systems The primary systems object
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Settings Menu state
     *
     */
    virtual ~SettingsMenu() = default;

    /**
     * @brief Returns "SettingsMenu"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Activates the state
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Deactivates the state
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the state and menus and whatnot
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the new game features
     *
     * @param engine The game engine
     * @param lag Time elapsed not accounted for in update
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    enum struct MenuState {
        TopMenu,

        VideoMenu,
        VideoSelectMode,

        AudioMenu,
        AudioSelectVolume,

        ControlsTopMenu,
        ControlsKBMMenu,
        ControlsPadMenu,
        ControlsBindingControl
    };

    MenuState state;
    bl::resource::Resource<sf::Texture>::Ref bgndTexture;
    sf::Sprite background;
    sf::Text hint;

    core::input::MenuDriver inputDriver;
    bl::menu::Menu topMenu;

    bl::menu::Menu videoMenu;
    bl::menu::TextItem::Ptr windowModeTextItem;
    bl::menu::SubmenuItem::Ptr windowModeDropdownItem;
    bl::menu::TextItem::Ptr fullscreenItem;
    bl::menu::TextItem::Ptr windowedItem;
    bl::menu::ToggleTextItem::Ptr vsyncItem;

    bl::menu::Menu audioMenu;
    bl::menu::ToggleTextItem::Ptr muteItem;
    bl::menu::TextItem::Ptr volumeItem;
    core::system::hud::QtyEntry volumeEntry;

    bl::menu::Menu controlsTopMenu;
    bl::input::Configurator ctrlConfigurator;
    unsigned int bindingCtrl;
    bool bindingKbm;
    bl::menu::TextItem* ctrlItem;

    bl::menu::Menu controlsKbmMenu;
    bl::menu::Menu controlsPadMenu;

    SettingsMenu(core::system::Systems& systems);

    void enterState(MenuState state);
    void setHint(const std::string& message);

    void onWindowModeOpen();
    bool isFullscreen() const;
    std::string windowModeString() const;
    void onWindowModeChange(bool fullscreen);
    void onVsyncUpdate();

    void startBindControl(bool kbm, unsigned int ctrl);
    std::string ctrlString(unsigned int ctrl, bool kbm) const;

    void back();
    void close();

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace state
} // namespace game

#endif
