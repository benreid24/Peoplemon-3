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
     * @brief Returns the position of the center of the camera
     *
     */
    const sf::Vector2f& getPosition() const;

    /**
     * @brief Returns the size factor of the camera. Size is treated as a factor of the window
     *        height stored in Properties
     *
     */
    float getSize() const;

    /**
     * @brief Sets the center and size of the given view based on the camera's position and size.
     *        Constrains the position of the view as best as possible to stay within the render area
     *
     * @param view The view to update
     * @param renderArea The size of the area being rendered
     */
    void configureView(sf::View& view, const sf::Vector2f& renderArea);

protected:
    sf::Vector2f position;
    float size;

    /**
     * @brief Construct a new Camera object with sane defaults
     *
     */
    Camera();

    /**
     * @brief Creates the camera with the initial position and size
     *
     * @param view The initial position to use
     * @param size The size to start at
     */
    Camera(const sf::Vector2f& position, float size);
};

} // namespace camera
} // namespace system
} // namespace core

#endif
