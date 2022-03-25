#ifndef CORE_BATTLES_VIEW_PEOPLEMONANIMATION_HPP
#define CORE_BATTLES_VIEW_PEOPLEMONANIMATION_HPP

#include <BLIB/Resources.hpp>
#include <Core/Battles/Commands/Animation.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace battle
{
namespace view
{
/**
 * @brief Helper for rendering peoplemon in battle and animating them
 *
 * @ingroup Battles
 *
 */
class PeoplemonAnimation {
public:
    /**
     * @brief Which position the peoplemon is in
     *
     */
    enum Position { Player, Opponent };

    /**
     * @brief Construct a new Peoplemon Animation utility
     *
     * @param position Which position and orientation for the peoplemon
     */
    PeoplemonAnimation(Position position);

    /**
     * @brief Sets the specific peoplemon graphic and resets to hidden state
     *
     * @param ppl The peoplemon to render
     */
    void setPeoplemon(pplmn::Id ppl);

    /**
     * @brief Begins playing the given animation
     *
     * @param type The animation to play
     */
    void triggerAnimation(Animation::Type type);

    /**
     * @brief Returns true if the animation has completed, false if in progress
     *
     */
    bool completed() const;

    /**
     * @brief Updates the playing animation
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Renders the peoplemon animation
     *
     * @param target The target to render to
     * @param lag Time elapsed not accounted for in update
     */
    void render(sf::RenderTarget& target, float lag);

private:
    enum struct State { Hidden, Static, Playing };

    State state;
    Animation::Type type;
    float slideAmount;
    sf::Vector2f shakeOff;
    // TODO - stuff for flashes etc
};

} // namespace view
} // namespace battle
} // namespace core

#endif