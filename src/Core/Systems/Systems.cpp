#include <Core/Systems/Systems.hpp>

namespace core
{
namespace systems
{
Systems::Systems(bl::engine::Engine& engine)
: _engine(engine) {}

const bl::engine::Engine& Systems::engine() const { return _engine; }

bl::engine::Engine& Systems::engine() { return _engine; }

} // namespace systems
} // namespace core
