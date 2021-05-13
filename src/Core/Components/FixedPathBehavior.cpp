#include <Core/Components/FixedPathBehavior.hpp>

namespace core
{
namespace component
{
FixedPathBehavior::FixedPathBehavior(const file::Behavior::Path& path)
: path(path)
, currentPace(0)
, currentStep(0)
, backwards(false) {}

void FixedPathBehavior::update(Position& position, Controllable& controller) {
    if (path.paces.empty()) return;

    const Direction moveDir = backwards ? oppositeDirection(path.paces[currentPace].direction) :
                                          path.paces[currentPace].direction;

    if (position.direction != moveDir) { controller.processControl(moveCommand(moveDir)); }
    else {
        if (controller.processControl(moveCommand(moveDir))) {
            if (backwards) {
                if (currentStep == 0) {
                    if (currentPace == 0) { backwards = false; }
                    else {
                        --currentPace;
                        currentStep = path.paces[currentPace].steps - 1;
                    }
                }
                else {
                    --currentStep;
                }
            }
            else {
                ++currentStep;
                if (currentStep >= path.paces[currentPace].steps) {
                    currentStep = 0;
                    ++currentPace;
                    if (currentPace >= path.paces.size()) {
                        if (path.reverse) {
                            backwards   = true;
                            currentPace = path.paces.size() - 1;
                            currentStep = path.paces.back().steps - 1;
                        }
                        else {
                            currentPace = 0;
                        }
                    }
                }
            }
        }
    }
}

} // namespace component
} // namespace core
