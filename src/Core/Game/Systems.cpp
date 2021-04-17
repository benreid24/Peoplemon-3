#include <Core/Game/Systems.hpp>

namespace core
{
namespace game
{
Systems::Systems(bl::engine::Engine& engine)
: _engine(engine) {}

const bl::engine::Engine& Systems::engine() const { return _engine; }

bl::engine::Engine& Systems::engine() { return _engine; }

} // namespace game
} // namespace core
