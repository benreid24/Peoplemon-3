#ifndef CORE_BATTLES_MOVEANIMATION_HPP
#define CORE_BATTLES_MOVEANIMATION_HPP

#include <BLIB/Graphics/Animation2D.hpp>
#include <BLIB/Render/Scenes/CodeScene.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Peoplemon/BattlePeoplemon.hpp>
#include <Core/Peoplemon/MoveId.hpp>

namespace core
{
namespace battle
{
namespace view
{
/**
 * @brief Utility for rendering move animations
 *
 * @ingroup Battles
 */
class MoveAnimation {
public:
    /**
     * @brief Which peoplemon is using the move
     */
    enum User { Player, Opponent };

    /**
     * @brief Construct a new Move Animation helper
     *
     * @param engine The game engine instance
     */
    MoveAnimation(bl::engine::Engine& engine);

    /**
     * @brief Initializes the animations
     *
     * @param scene The scene to use
     */
    void init(bl::rc::scene::CodeScene* scene);

    /**
     * @brief Loads the move animations into the resource manager for the given peoplemon
     *
     * @param player The player's peoplemon that is out
     * @param opponent The other peoplemon that is out
     */
    void ensureLoaded(const pplmn::BattlePeoplemon& player, const pplmn::BattlePeoplemon& opponent);

    /**
     * @brief Begins playing the given move animation
     *
     * @param user The peoplemon using the move
     * @param move The move being used
     */
    void playAnimation(User user, pplmn::MoveId move);

    /**
     * @brief Returns whether or not the animation has completed playing
     */
    bool completed() const;

    /**
     * @brief Renders the move animation background
     *
     * @param ctx The render context
     */
    void renderBackground(bl::rc::scene::CodeScene::RenderContext& ctx);

    /**
     * @brief Renders the move animation foreground
     *
     * @param ctx The render context
     */
    void renderForeground(bl::rc::scene::CodeScene::RenderContext& ctx);

private:
    struct Anim {
        pplmn::MoveId move;
        bl::gfx::Animation2D background;
        bl::gfx::Animation2D foreground;
        bool valid;

        Anim();
        bool init(bl::engine::Engine& engine, bl::rc::scene::CodeScene* scene, User user,
                  pplmn::MoveId move);
        void play();
        bool finished() const;
    };

    bl::engine::Engine& engine;
    bl::rc::scene::CodeScene* scene;
    Anim playerAnims[4];
    Anim opponentAnims[4];
    Anim extraMove;
    Anim* playing;
};

} // namespace view
} // namespace battle
} // namespace core

#endif
