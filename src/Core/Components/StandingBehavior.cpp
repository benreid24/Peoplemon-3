#include <Core/Components/StandingBehavior.hpp>

namespace core
{
namespace component
{
StandingBehavior::StandingBehavior(Direction faceDir)
: dir(faceDir) {}

void StandingBehavior::update(Position& position, Controllable& controller) {
    if (position.direction != dir) { controller.processControl(moveCommand(dir)); }
}

} // namespace component
} // namespace core
