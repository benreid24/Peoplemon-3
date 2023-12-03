#include <Game/States/State.hpp>

namespace game
{
namespace state
{
State::State(core::system::Systems& s)
: bl::engine::State(bl::engine::StateMask::All)
, systems(s) {}

} // namespace state
} // namespace game
