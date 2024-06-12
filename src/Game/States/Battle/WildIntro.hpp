#ifndef GAME_STATE_BATTLE_WILDINTRO_HPP
#define GAME_STATE_BATTLE_WILDINTRO_HPP

#include <BLIB/Graphics.hpp>
#include <Game/States/BattleWrapperState.hpp>

namespace game
{
namespace state
{
namespace intros
{
class WildSequence : public SequenceBase {
public:
    WildSequence();
    virtual ~WildSequence() = default;
    virtual void start(bl::engine::Engine& engine) override;
    virtual void update(float dt) override;
    virtual bool finished() const override;

private:
    bl::engine::Engine* e;
    float time;
    float shuffleTime;
    bl::gfx::Rectangle background;
    void* uniform;
};

} // namespace intros
} // namespace state
} // namespace game

#endif
