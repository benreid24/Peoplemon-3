#ifndef CORE_BATTLES_VIEW_PLAYERMENU_HPP
#define CORE_BATTLES_VIEW_PLAYERMENU_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Battles/Commands/TurnAction.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Player/Input/MenuDriver.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace battle
{
namespace view
{
/**
 * @brief The player menu for battles
 *
 * @ingroup Battles
 *
 */
class PlayerMenu {
public:
    /**
     * @brief Construct a new Player Menu
     *
     */
    PlayerMenu();

    /**
     * @brief Updates the menu for the given peoplemon
     *
     */
    void setPeoplemon(const pplmn::BattlePeoplemon& ppl);

    /**
     * @brief Resets the menu to the turn start state
     *
     */
    void beginTurn();

    /**
     * @brief Opens the menu to select a peoplemon to send out
     *
     */
    void chooseFaintReplacement();

    /**
     * @brief Returns whether or not the action has been selected
     *
     */
    bool turnActionSelected() const;

    /**
     * @brief The selected turn action
     *
     */
    TurnAction selectedAction() const;

    /**
     * @brief Returns whether or not the sub action has been selected
     *
     */
    bool subActionSelected() const;

    /**
     * @brief Returns the selected move index
     *
     */
    int selectedMove() const;

    /**
     * @brief Returns the selected item
     *
     */
    item::Id selectedItem() const;

    /**
     * @brief Returns the selected peoplemon index
     *
     */
    int selectedPeoplemon() const;

    /**
     * @brief Processes player input to update the menu state
     *
     */
    void handleInput(component::Command input);

    /**
     * @brief Renders the menu to the given target
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

private:
    player::input::MenuDriver menuDriver;
    bl::menu::Menu actionMenu;
    bl::menu::Menu moveMenu;

    bl::resource::Resource<sf::Texture>::Ref moveBoxTxtr;
    sf::Sprite moveBox;
    sf::Text movePP;
    sf::Text movePwr;
    sf::Text moveAcc;
};

} // namespace view
} // namespace battle
} // namespace core

#endif