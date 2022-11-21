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

    core::input::MenuDriver inputDriver;
    bl::menu::Menu topMenu;

    bl::menu::Menu videoMenu;
    bl::menu::TextItem::Ptr windowModeItem;

    bl::menu::Menu audioMenu;
    bl::menu::TextItem::Ptr muteItem;
    bl::menu::TextItem::Ptr volumeItem;

    bl::menu::Menu controlsTopMenu;

    bl::menu::Menu controlsKbmMenu;
    bl::menu::TextItem::Ptr kbmUpItem;
    bl::menu::TextItem::Ptr kbmRightItem;
    bl::menu::TextItem::Ptr kbmDownItem;
    bl::menu::TextItem::Ptr kbmLeftItem;
    bl::menu::TextItem::Ptr kbmSprintItem;
    bl::menu::TextItem::Ptr kbmInteractItem;
    bl::menu::TextItem::Ptr kbmBackItem;
    bl::menu::TextItem::Ptr kbmPauseItem;

    bl::menu::Menu controlsPadMenu;
    bl::menu::TextItem::Ptr padUpItem;
    bl::menu::TextItem::Ptr padRightItem;
    bl::menu::TextItem::Ptr padDownItem;
    bl::menu::TextItem::Ptr padLeftItem;
    bl::menu::TextItem::Ptr padSprintItem;
    bl::menu::TextItem::Ptr padInteractItem;
    bl::menu::TextItem::Ptr padBackItem;
    bl::menu::TextItem::Ptr padPauseItem;

    SettingsMenu(core::system::Systems& systems);

    void enterState(MenuState state);

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
