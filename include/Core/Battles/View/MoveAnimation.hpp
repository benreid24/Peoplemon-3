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
     * @param ppl The peoplemon to load for
     */
    void ensureLoaded(const core::pplmn::BattlePeoplemon& ppl);

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
     * @brief Renders the move animation
     *
     * @param target The target to render to
     * @param lag Time elapsed not accounted for in update
     */
    void render(sf::RenderTarget& target, float lag) const;

private:
    bl::resource::Resource<bl::gfx::AnimationData>::Ref playingSrc;
    bl::gfx::Animation playing;
};

} // namespace view
} // namespace battle
} // namespace core

#endif