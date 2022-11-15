#ifndef CORE_COMPONENTS_FIXEDPATHBEHAVIOR_HPP
#define CORE_COMPONENTS_FIXEDPATHBEHAVIOR_HPP

#include <Core/Components/Controllable.hpp>
#include <Core/Files/Behavior.hpp>

namespace core
{
namespace component
{
/**
 * @brief Add this component to an entity to make it follow a fixed path
 *
 * @ingroup Components
 *
 */
class FixedPathBehavior {
public:
    /**
     * @brief Construct a new Fixed Path Behavior component
     *
     * @param path The path to follow
     */
    FixedPathBehavior(const file::Behavior::Path& path);

    /**
     * @brief Updates the entity along the path
     *
     * @param position The position component of the entity
     * @param controller The controllable component of the entity
     */
    void update(Position& position, Controllable& controller, float);

private:
    const file::Behavior::Path path;
    bool backwards;
    unsigned int currentPace;
    unsigned int currentStep;
};

} // namespace component
} // namespace core

#endif
