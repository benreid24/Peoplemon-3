#ifndef GAME_STATE_EVOLUTION_HPP
#define GAME_STATE_EVOLUTION_HPP

#include <BLIB/Media/Shapes.hpp>
#include <BLIB/Particles.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Game state for when a Peoplemon evolves into another one
 *
 * @ingroup States
 *
 */
class Evolution
: public State
, public bl::input::Listener {
public:
    static bl::engine::State::Ptr create(core::system::Systems& systems,
                                         core::pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Destroy the Evolution state
     *
     */
    virtual ~Evolution() = default;

    /**
     * @brief Returns "Evolution"
     *
     */
    const char* name() const override;

    /**
     * @brief Subscribes to player input
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Unsubscribes from player input
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the evolution process
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the evolution process
     *
     * @param engine The game engine
     * @param lag Time, in seconds, not accounted for in update()
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    core::pplmn::OwnedPeoplemon& ppl;

    bl::resource::Ref<sf::Texture> bgndTxtr;
    bl::resource::Ref<sf::Texture> oldTxtr;
    bl::resource::Ref<sf::Texture> newTxtr;
    sf::Sprite background;
    sf::Sprite oldThumb;
    sf::Sprite newThumb;

    enum struct AnimState {
        IntroMsg,
        OldFadeOut,
        SizeOscillating,
        NewFadeIn,
        EvolvedMsg,
        CancelConfirm,
        CancelMsg
    };

    struct Spark {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float radius;
        float time;
        float lifetime;
    };

    AnimState state;
    AnimState cancelPriorState;
    bl::particle::System<Spark> sparks;
    bl::shapes::GradientCircle spark;
    union {
        float fadeColor;
        float oscillateTime;
    };

    Evolution(core::system::Systems& systems, core::pplmn::OwnedPeoplemon& ppl);

    void spawnSpark(Spark* obj);

    void messageDone();
    void onCancelConfirm(const std::string& choice);
    virtual bool observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                         bool fromEvent) override;
};

} // namespace state
} // namespace game

#endif
