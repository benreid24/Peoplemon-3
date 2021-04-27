#ifndef CORE_SYSTEMS_CAMERAS_CAMERA_HPP
#define CORE_SYSTEMS_CAMERAS_CAMERA_HPP

#include <SFML/Graphics/View.hpp>
#include <memory>

namespace core
{
namespace system
{
class Systems;

/// Collection of cameras for controlling what is rendered
namespace camera
{
/**
 * @brief Base class for all cameras
 *
 * @ingroup Cameras
 *
 */
class Camera {
public:
    /// Pointer type to camera
    using Ptr = std::shared_ptr<Camera>;

    /**
     * @brief Destroy the Camera object
     *
     */
    virtual ~Camera() = default;

    /**
     * @brief Returns whether or not the camera is still valid
     *
     * @return True if the camera may still be used, false otherwise
     */
    virtual bool valid() const = 0;

    /**
     * @brief Updates the camera
     *
     * @param systems The primary systems object
     * @param dt Time elapsed since last call to update
     */
    virtual void update(Systems& systems, float dt) = 0;

    /**
     * @brief Returns the view to be used for rendering
     *
     */
    const sf::View& getView() const;

protected:
    /// The view the camera should use and update in value
    sf::View view;

    /**
     * @brief Contrains the contained view to the renderable region of the world
     *
     * @param systems The primary systems object
     */
    void constrainView(Systems& systems);
};

} // namespace camera
} // namespace system
} // namespace core

#endif
