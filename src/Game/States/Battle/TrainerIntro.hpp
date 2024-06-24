#ifndef GAME_STATE_BATTLE_TRAINERINTRO_HPP
#define GAME_STATE_BATTLE_TRAINERINTRO_HPP

#include <BLIB/Graphics.hpp>
#include <Game/States/BattleWrapperState.hpp>

namespace game
{
namespace state
{
namespace intros
{
class TrainerSequence : public SequenceBase {
public:
    TrainerSequence();
    virtual ~TrainerSequence() = default;
    virtual void start(bl::engine::Engine& engine) override;
    virtual void update(float dt) override;
    virtual bool finished() const override;

private:
    bl::engine::Engine* e;
    float time;
    bl::rc::res::TextureRef ballTxtr;
    bl::gfx::Sprite ball;
    bl::gfx::Rectangle background;
    void* uniform;
};

} // namespace intros
} // namespace state
} // namespace game

#endif
