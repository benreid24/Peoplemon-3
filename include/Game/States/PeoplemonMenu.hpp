#ifndef GAME_STATES_PEOPLEMONMENU_HPP
#define GAME_STATES_PEOPLEMONMENU_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Player/Input/MenuDriver.hpp>
#include <Game/Menus/PeoplemonButton.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Provides the menu for viewing and selecting Peoplemon
 *
 * @ingroup States
 *
 */
class PeoplemonMenu : public State {
public:
    /**
     * @brief Helper struct to ferry data between states utilizing this menu
     *
     */
    struct ContextData {
        /// The index of the peoplemon currently out
        unsigned int outNow;

        /// The index of the selected peoplemon. -1 means no selection
        int chosen;
    };

    /**
     * @brief Represents where the menu is being opened from
     *
     */
    enum struct Context { BattleSwitch, BattleFaint, StorageSelect, PauseMenu };

    /**
     * @brief Creates a new PeoplemonMenu state
     *
     * @param systems The main game systems
     * @param ctx The context the menu is being used in
     * @param data Optional struct to place the selected peoplemon in
     *
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems, Context ctx,
                                         ContextData* data = nullptr);

    /// Cleans up all resources
    virtual ~PeoplemonMenu() = default;

    /**
     * @brief Returns "PeoplemonMenu";
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Subscribes the menu to the engine event bus
     *
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Unsubscribes the menu to the engine event bus
     *
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Does nothing
     *
     */
    virtual void update(bl::engine::Engine&, float dt) override;

    /**
     * @brief Renders the menu and its background
     *
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    enum MenuState {
        Browsing,
        SelectingMove,
        Moving,
        ShowingMessage
    };

    const Context context;
    ContextData* const data;
    sf::View oldView;

    MenuState state;
    unsigned int mover1;
    unsigned int mover2;
    sf::Vector2f moveVel;
    sf::Vector2f mover1Dest;
    sf::Vector2f mover2Dest;

    bl::resource::Resource<sf::Texture>::Ref backgroundTxtr;
    sf::Sprite background;

    bl::menu::Menu menu;
    core::player::input::MenuDriver inputDriver;
    menu::PeoplemonButton::Ptr buttons[6];
    bl::menu::ImageItem::Ptr backBut;

    bl::menu::Menu actionMenu;
    bl::menu::Item* actionRoot;
    bool actionOpen;

    PeoplemonMenu(core::system::Systems& systems, Context ctx, ContextData* data);

    void connectButtons();

    void selected(menu::PeoplemonButton* but);

    void chosen(); // store or battle switch
    void showInfo();
    void startMove();
    void cleanupMove(bool completed);
    void takeItem();
    void resetAction();
    void messageDone();
};

} // namespace state
} // namespace game

#endif
