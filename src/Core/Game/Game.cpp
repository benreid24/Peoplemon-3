#include <Core/Game/Game.hpp>

namespace core
{
namespace game
{
Game::Game(bl::engine::Engine& engine)
: _engine(engine) {}

const bl::engine::Engine& Game::engine() const { return _engine; }

bl::engine::Engine& Game::engine() { return _engine; }

} // namespace game
} // namespace core
