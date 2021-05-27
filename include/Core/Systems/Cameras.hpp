#ifndef CORE_SYSTEMS_CAMERAS_HPP
#define CORE_SYSTEMS_CAMERAS_HPP

#include <Core/Systems/Cameras/Camera.hpp>
#include <stack>

/**
 * @addtogroup Cameras
 * @ingroup Systems
 * @brief Collection of cameras and utilities for controlling what is rendered
 *
 */

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Primary camera system. Manages the currently active camera and the stack of previously
 *        active cameras
 *
 * @ingroup Systems
 * @ingroup Cameras
 *
 */
class Cameras {
public:
    /**
     * @brief Construct a new Cameras system
     *
     * @param owner The primary Systems object
     */
    Cameras(Systems& owner);

    /**
     * @brief Returns the currently active camera
     *
     */
    camera::Camera::Ptr activeCamera();

    /**
     * @brief Adds a new camera to immediately use, saving the previous active camera
     *
     * @param camera The new camera to use
     */
    void pushCamera(camera::Camera::Ptr camera);

    /**
     * @brief Replaces the current active camera with a new one. The previous camera is not able to
     *        be returned to
     *
     * @param camera The new camera to use
     */
    void replaceCamera(camera::Camera::Ptr camera);

    /**
     * @brief Removes the current active camera and resets to the previous one
     *
     */
    void popCamera();

    /**
     * @brief Clears the current camera stack and pushes the given camera to it
     *
     * @param camera The camera to replace all current cameras with
     */
    void clearAndReplace(camera::Camera::Ptr camera);

    /**
     * @brief Returns the view to use for rendering
     *
     * @param viewport The viewport to constrain the view within. Useful for letterboxing
     *
     */
    const sf::View& getView(const sf::FloatRect& viewport) const;

    /**
     * @brief Returns the area to be rendered by the active camera
     *
     */
    sf::FloatRect getArea() const;

    /**
     * @brief Updates the current active camera. Pops invalidated cameras as necessary
     *
     * @param dt Time elapsed in seconds since last call to update
     */
    void update(float dt);

private:
    Systems& owner;
    std::stack<camera::Camera::Ptr> cameras;
};

} // namespace system
} // namespace core

#endif
