#ifndef CORE_BATTLES_VIEW_PEOPLEMONANIMATION_HPP
#define CORE_BATTLES_VIEW_PEOPLEMONANIMATION_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Particles/System.hpp>
#include <BLIB/Render/Scenes/CodeScene.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Battles/Commands/Animation.hpp>
#include <Core/Battles/View/PeoplemonSpark.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

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
 */
class PeoplemonAnimation {
public:
    /**
     * @brief Which position the peoplemon is in
     */
    enum Position { Player, Opponent };

    /**
     * @brief Construct a new Peoplemon Animation utility
     *
     * @param engine The game engine instance
     * @param position Which position and orientation for the peoplemon
     */
    PeoplemonAnimation(bl::engine::Engine& engine, Position position);

    /**
     * @brief Initializes rendering resources
     *
     * @param scene The scene to use
     */
    void init(bl::rc::scene::CodeScene* scene);

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
     * @param ctx The render context
     */
    void render(bl::rc::scene::CodeScene::RenderContext& ctx);

private:
    enum struct State { Hidden, Static, Playing };

    bl::engine::Engine& engine;
    bl::rc::scene::CodeScene* scene;
    const Position position;
    const VkViewport viewport;
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
    glm::vec2 shakeOff;

    bl::rc::res::TextureRef ballTxtr;
    bl::rc::res::TextureRef ballOpenTxtr;
    bl::gfx::Sprite ball;
    bl::gfx::Circle ballFlash;

    bl::pcl::ParticleManager<PeoplemonSpark>* sparks;
    SparkExplosionEmitter* sparkExplosionEmitter;
    bl::pcl::ParticleManager<PeoplemonSpark>* implosion;
    SparkImplosionEmitter* sparkImplosianEmitter;
    bl::gfx::Rectangle screenFlash;

    bl::rc::res::TextureRef statTxtr;
    bl::gfx::Sprite statArrow;
    float arrowOffset;
    bool renderBall;

    enum struct BallThrowState { Arcing, Eating, Bouncing, CloneFading } throwState;
    bl::rc::res::TextureRef throwBallTxtr;
    bl::gfx::Sprite throwBall;
    bl::gfx::Sprite clone;
    bl::gfx::Sprite* toEat;

    void setBallTexture(bl::rc::res::TextureRef& t);
    void setThrowBallTxtr(bl::rc::res::TextureRef& t);

    bl::resource::Ref<bl::gfx::a2d::AnimationData> annoySrc;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> confuseSrc;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> frozenSrc;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> frustrationSrc;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> sleepSrc;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> stickySrc;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> trappedSrc;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> jumpedSrc;
    bl::gfx::Animation2D ailmentAnim;

    void updateAilmentAnimation(pplmn::Ailment ail);
    void updateAilmentAnimation(pplmn::PassiveAilment ail);
    void recreateAilmentAnimation(bl::resource::Ref<bl::gfx::a2d::AnimationData>& src);

    bl::rc::res::TextureRef txtr;
    bl::gfx::Sprite peoplemon;
    glm::vec2 scale;
};

} // namespace view
} // namespace battle
} // namespace core

#endif
