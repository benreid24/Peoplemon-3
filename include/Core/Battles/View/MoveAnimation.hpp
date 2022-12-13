#ifndef CORE_BATTLES_MOVEANIMATION_HPP
#define CORE_BATTLES_MOVEANIMATION_HPP

#include <BLIB/Media/Graphics/Animation.hpp>
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
 * @ingroup
 *
 */
class MoveAnimation {
public:
    /**
     * @brief Which peoplemon is using the move
     *
     */
    enum User { Player, Opponent };

    /**
     * @brief Construct a new Move Animation helper
     *
     */
    MoveAnimation();

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
     *
     */
    bool completed() const;

    /**
     * @brief Updates the playing animation
     *
     * @param dt Time elapsed
     */
    void update(float dt);

    /**
     * @brief Renders the move animation background
     *
     * @param target The target to render to
     * @param lag Time elapsed not accounted for in update
     */
    void renderBackground(sf::RenderTarget& target, float lag) const;

    /**
     * @brief Renders the move animation foreground
     *
     * @param target The target to render to
     * @param lag Time elapsed not accounted for in update
     */
    void renderForeground(sf::RenderTarget& target, float lag) const;

private:
    bl::resource::Ref<bl::gfx::AnimationData> bgSrc;
    bl::resource::Ref<bl::gfx::AnimationData> fgSrc;
    bl::gfx::Animation background;
    bl::gfx::Animation foreground;
};

} // namespace view
} // namespace battle
} // namespace core

#endif
