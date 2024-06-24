#ifndef CORE_BATTLES_VIEW_STATBOXES_HPP
#define CORE_BATTLES_VIEW_STATBOXES_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Render/Scenes/CodeScene.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>

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
 */
class StatBoxes {
public:
    /**
     * @brief Construct a new Stat Boxes component
     *
     * @param engine The game engine instance
     */
    StatBoxes(bl::engine::Engine& engine);

    /**
     * @brief Creates entities and adds them to the scene
     *
     * @param scene The scene to add to
     */
    void init(bl::rc::scene::CodeScene* scene);

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
     */
    bool synced() const;

    /**
     * @brief Renders the boxes
     *
     * @param ctx The render context
     */
    void render(bl::rc::scene::CodeScene::RenderContext& ctx);

private:
    enum struct State { Hidden, Sliding, Showing };

    bl::engine::Engine& engine;

    pplmn::BattlePeoplemon* localPlayer;
    pplmn::BattlePeoplemon* opponent;
    State pState;
    State oState;

    bl::rc::res::TextureRef opBoxTxtr;
    bl::rc::res::TextureRef lpBoxTxtr;
    bl::gfx::Sprite opBox;
    bl::gfx::Sprite lpBox;

    bl::gfx::Rectangle opHpBar;
    float opHpBarTarget;
    bl::gfx::Text opName;
    bl::gfx::Text opLevel;
    bl::gfx::Sprite opAil;

    bl::gfx::Rectangle lpHpBar;
    float lpHpBarTarget;
    bl::gfx::Rectangle lpXpBar;
    float lpXpBarTarget;
    bl::gfx::Text lpName;
    bl::gfx::Text lpHp;
    bl::gfx::Text lpLevel;
    bl::gfx::Sprite lpAil;

    bl::rc::res::TextureRef ailmentTexture(pplmn::Ailment ailment);
};

} // namespace view
} // namespace battle
} // namespace core

#endif
