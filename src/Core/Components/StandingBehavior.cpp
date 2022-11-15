#include <Core/Components/StandingBehavior.hpp>

namespace core
{
namespace component
{
StandingBehavior::StandingBehavior(Direction faceDir)
: dir(faceDir) {}

void StandingBehavior::update(Position& position, Controllable& controller, float) {
    if (position.direction != dir) { controller.processControl(moveControlFromDirection(dir)); }
}

} // namespace component
} // namespace core
