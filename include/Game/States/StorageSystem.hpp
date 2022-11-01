#ifndef GAME_STATES_STORAGESYSTEM_HPP
#define GAME_STATES_STORAGESYSTEM_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Peoplemon/StoredPeoplemon.hpp>
#include <Core/Player/Input/Listener.hpp>
#include <Game/Menus/StorageCursor.hpp>
#include <Game/Menus/StorageGrid.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Game state for the peoplemon storage system menu
 *
 * @ingroup States
 *
 */
class StorageSystem
: public State
, private core::player::input::Listener {
public:
    /**
     * @brief Create a new storage system menu
     *
     * @param systems The main game states
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Storage System object
     *
     */
    virtual ~StorageSystem() = default;

    /**
     * @brief Returns "StorageSystem"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Initializes the menu and subscribes to event buses
     *
     * @param engine The game engine instance
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Unsubscribes from event buses and fires a StorageSystemClosed event
     *
     * @param engine The game engine instance
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the menu state
     *
     * @param engine The game engine instance
     * @param dt Time elapsed, in seconds, since last call to update
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the menu to the game window
     *
     * @param engine The game engine instance
     * @param lag Time elapsed in seconds not accounted for in update
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    enum struct MenuState {
        // entry states
        ChooseAction,
        PlacingPeoplemon,

        // browse states
        BrowsingBox,
        BoxSliding,
        CursorMoving,

        // context menu states
        BrowseMenuOpen,
        MovingPeoplemon,
        WaitingContextMessage,
        WaitingReleaseConfirm,

        // Waiting on other states
        WaitingDeposit
    };

    MenuState state;
    MenuState prevState;
    int currentBox;
    core::pplmn::StoredPeoplemon* hovered;
    bool showInfo;
    sf::Vector2i selectPos;
    bool closeMenuAfterMessage;

    bl::resource::Resource<sf::Texture>::Ref backgroundTxtr;
    sf::Sprite background;
    sf::View view;
    sf::View boxView;

    core::player::input::MenuDriver menuDriver;
    menu::StorageGrid activeGrid;
    menu::StorageGrid slidingOutGrid;
    menu::StorageCursor cursor;
    float slideOffset;
    float slideVel;

    bl::audio::AudioSystem::Handle cursorMoveSound;
    bl::audio::AudioSystem::Handle pageSlideSound;
    bl::audio::AudioSystem::Handle pageSlideFailSound;

    bl::resource::Resource<sf::Texture>::Ref leftArrowTxtr;
    bl::resource::Resource<sf::Texture>::Ref rightArrowTxtr;
    sf::Sprite leftArrow;
    sf::Sprite rightArrow;
    sf::Text boxTitle;

    bl::resource::Resource<sf::Texture>::Ref thumbTxtr;
    sf::Sprite thumbnail;
    sf::Text nickname;
    sf::Text level;
    sf::Text itemLabel;
    sf::Text itemName;

    bl::menu::Menu actionMenu;
    bl::menu::Item::Ptr withdrawActionItem;
    bl::menu::Menu contextMenu;

    int depositedPeoplemon;
    sf::Vector2i ogMovePos;

    StorageSystem(core::system::Systems& systems);

    // entry actions
    void startDeposit();
    void startBrowse();
    void close();

    // browse actions
    void onCursor(const sf::Vector2i& pos);
    void onHover(core::pplmn::StoredPeoplemon* peoplemon);
    void onSelect(const sf::Vector2i& position);
    void boxLeft();
    void boxRight();

    // context actions
    void onWithdraw();
    void onStartMove();
    void onTakeItem();
    void onRelease();
    void onCloseContextMenu();

    virtual void process(core::component::Command control) override;

    void enterState(MenuState state);
    void finishBoxChange();
    void onReleaseConfirm(const std::string& choice);
    void onMessageDone();
    void updatePeoplemonInfo(const core::pplmn::OwnedPeoplemon& ppl);
    void showContextMessage(const std::string& msg, bool closeMenu = true);
};

} // namespace state
} // namespace game

#endif
