#ifndef CORE_BATTLES_VIEW_PEOPLEMONANIMATION_HPP
#define CORE_BATTLES_VIEW_PEOPLEMONANIMATION_HPP

#include <BLIB/Media/Graphics/Animation.hpp>
#include <BLIB/Media/Graphics/Flashing.hpp>
#include <BLIB/Media/Shapes/GradientCircle.hpp>
#include <BLIB/Particles/System.hpp>
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
     * @brief Sets up the subviews from the view used during battle
     *
     * @param parentView The view to be used
     */
    void configureView(const sf::View& parentView);

    /**
     * @brief Sets the specific peoplemon graphic and resets to hidden state
     *
     * @param ppl The peoplemon to render
     */
    void setPeoplemon(pplmn::Id ppl);

    /**
     * @brief Begins playing the given animation
     *
     * @param anim The animation to play
     */
    void triggerAnimation(const cmd::Animation& anim);

    /**
     * @brief Begins playing the given animation by type only
     *
     * @param type The animation to play
     */
    void triggerAnimation(cmd::Animation::Type type);

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
    void render(sf::RenderTarget& target, float lag) const;

private:
    enum struct State { Hidden, Static, Playing };

    struct Spark {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float radius;
        float time;
        float lifetime;
    };

    const Position position;
    sf::Vector2f offset;
    sf::View view;
    State state;
    cmd::Animation::Type type;
    union {
        float slideAmount;
        float alpha;
        float shakeTime;
        float ballTime;
        float arrowTime;
        float throwX;
        float bounceTime;
    };
    sf::Vector2f shakeOff;

    bl::resource::Ref<sf::Texture> ballTxtr;
    bl::resource::Ref<sf::Texture> ballOpenTxtr;
    sf::Sprite ball;
    bl::shapes::GradientCircle ballFlash;
    bl::particle::System<Spark> sparks;
    bl::particle::System<Spark> implosion;
    mutable bl::shapes::GradientCircle spark;
    sf::RectangleShape screenFlash;
    bl::resource::Ref<sf::Texture> statTxtr;
    sf::Sprite statArrow;
    float arrowOffset;
    sf::Vector2f implodeOrigin;
    bool renderBall;

    enum struct BallThrowState { Arcing, Eating, Bouncing, CloneFading } throwState;
    bl::resource::Ref<sf::Texture> throwBallTxtr;
    sf::Sprite throwBall;
    sf::Sprite clone;
    sf::Sprite* toEat;

    void setBallTexture(sf::Texture& t);
    void spawnSpark(Spark* obj);
    void spawnImplodeSpark(Spark* obj);
    void setThrowBallTxtr(sf::Texture& t);

    bl::resource::Ref<bl::gfx::AnimationData> annoySrc;
    bl::resource::Ref<bl::gfx::AnimationData> confuseSrc;
    bl::resource::Ref<bl::gfx::AnimationData> frozenSrc;
    bl::resource::Ref<bl::gfx::AnimationData> frustrationSrc;
    bl::resource::Ref<bl::gfx::AnimationData> sleepSrc;
    bl::resource::Ref<bl::gfx::AnimationData> stickySrc;
    bl::resource::Ref<bl::gfx::AnimationData> trappedSrc;
    bl::resource::Ref<bl::gfx::AnimationData> jumpedSrc;
    bl::gfx::Animation ailmentAnim;

    void updateAilmentAnimation(pplmn::Ailment ail);
    void updateAilmentAnimation(pplmn::PassiveAilment ail);

    bl::resource::Ref<sf::Texture> txtr;
    mutable sf::Sprite peoplemon;
    bl::gfx::Flashing flasher;
    sf::Vector2f scale;
};

} // namespace view
} // namespace battle
} // namespace core

#endif
