#ifndef GAME_STATE_EVOLUTION_HPP
#define GAME_STATE_EVOLUTION_HPP

#include <BLIB/Graphics.hpp>
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
 */
class Evolution
: public State
, public bl::input::Listener {
public:
    static bl::engine::State::Ptr create(core::system::Systems& systems,
                                         core::pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Destroy the Evolution state
     */
    virtual ~Evolution() = default;

    /**
     * @brief Returns "Evolution"
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
    virtual void update(bl::engine::Engine& engine, float dt, float) override;

    struct Spark {
        glm::vec2 position;
        glm::vec2 velocity;
        float radius;
        float time;
        float lifetime;
    };

private:
    core::pplmn::OwnedPeoplemon& ppl;

    bl::rc::res::TextureRef bgndTxtr;
    bl::rc::res::TextureRef oldTxtr;
    bl::rc::res::TextureRef newTxtr;
    bl::gfx::Sprite background;
    bl::gfx::Sprite oldThumb;
    bl::gfx::Sprite newThumb;

    enum struct AnimState {
        IntroMsg,
        OldFadeOut,
        SizeOscillating,
        NewFadeIn,
        EvolvedMsg,
        CancelConfirm,
        CancelMsg
    };

    AnimState state;
    AnimState cancelPriorState;
    bl::pcl::ParticleManager<Spark>* sparks;
    union {
        float fadeColor;
        float oscillateTime;
    };

    Evolution(core::system::Systems& systems, core::pplmn::OwnedPeoplemon& ppl);

    void messageDone();
    void onCancelConfirm(const std::string& choice);
    virtual bool observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                         bool fromEvent) override;
};

} // namespace state
} // namespace game

#endif
