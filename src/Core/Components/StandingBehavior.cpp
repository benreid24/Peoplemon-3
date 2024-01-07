#include <Core/Components/StandingBehavior.hpp>

namespace core
{
namespace component
{
StandingBehavior::StandingBehavior(bl::tmap::Direction faceDir)
: dir(faceDir) {}

void StandingBehavior::update(bl::tmap::Position& position, Controllable& controller) {
    if (position.direction != dir) { controller.processControl(input::fromDirection(dir)); }
}

} // namespace component
} // namespace core
