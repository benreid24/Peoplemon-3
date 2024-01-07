#include <Game/States/State.hpp>

namespace game
{
namespace state
{
State::State(core::system::Systems& s, bl::engine::StateMask::V mask)
: bl::engine::State(mask)
, systems(s) {}

} // namespace state
} // namespace game
