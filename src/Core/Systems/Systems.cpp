#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Systems::Systems(bl::engine::Engine& engine)
: _engine(engine)
, _clock(*this) {}

void Systems::update(float dt) { _clock.update(dt); }

const bl::engine::Engine& Systems::engine() const { return _engine; }

bl::engine::Engine& Systems::engine() { return _engine; }

const Clock& Systems::clock() const { return _clock; }

} // namespace system
} // namespace core
