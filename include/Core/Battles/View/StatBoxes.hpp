#ifndef CORE_BATTLES_VIEW_STATBOXES_HPP
#define CORE_BATTLES_VIEW_STATBOXES_HPP

#include <BLIB/Resources.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace battle
{
/// Collection of classes that compose the BattleView
namespace view
{
/**
 * @brief Renders both the player and opponent stat boxes
 *
 * @ingroup Battles
 *
 */
class StatBoxes {
public:
    /**
     * @brief Construct a new Stat Boxes component
     *
     */
    StatBoxes();

    /**
     * @brief Sets the opponent's peoplemon. Syncs right away
     *
     * @param ppl The opponent's peoplemon
     */
    void setOpponent(pplmn::BattlePeoplemon* ppl);

    /**
     * @brief Sets the player's peoplemon. Syncs right away
     *
     * @param ppl The player's peoplemon
     */
    void setPlayer(pplmn::BattlePeoplemon* ppl);

    /**
     * @brief Updates the UI to reflect the current peoplemon state
     *
     * @param fromSwitch True to sync from switch, false for regular sync
     *
     */
    void sync(bool fromSwitch = false);

    /**
     * @brief Updates bar sizes if they need to change
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Returns true if the view is in sync, false if an animation is in progress
     *
     */
    bool synced() const;

    /**
     * @brief Renders the boxes to the target
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

private:
    enum struct State { Hidden, Sliding, Showing };

    pplmn::BattlePeoplemon* localPlayer;
    pplmn::BattlePeoplemon* opponent;
    State pState;
    State oState;

    sf::Texture blank;
    bl::resource::Resource<sf::Texture>::Ref annoyTxtr;
    bl::resource::Resource<sf::Texture>::Ref frustTxtr;
    bl::resource::Resource<sf::Texture>::Ref stickyTxtr;
    bl::resource::Resource<sf::Texture>::Ref slpTxtr;
    bl::resource::Resource<sf::Texture>::Ref frzTxtr;

    bl::resource::Resource<sf::Texture>::Ref opBoxTxtr;
    bl::resource::Resource<sf::Texture>::Ref lpBoxTxtr;
    sf::Sprite opBox;
    sf::Sprite lpBox;

    sf::RectangleShape opHpBar;
    float opHpBarTarget;
    sf::Text opName;
    sf::Text opLevel;
    sf::Sprite opAil;

    sf::RectangleShape lpHpBar;
    float lpHpBarTarget;
    sf::RectangleShape lpXpBar;
    float lpXpBarTarget;
    sf::Text lpName;
    sf::Text lpHp;
    sf::Text lpLevel;
    sf::Sprite lpAil;

    const sf::Texture& ailmentTexture(pplmn::Ailment ailment) const;
};

} // namespace view
} // namespace battle
} // namespace core

#endif
