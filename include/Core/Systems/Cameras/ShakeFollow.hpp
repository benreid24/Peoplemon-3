#ifndef CORE_SYSTEMS_CAMERAS_SHAKEFOLLOW_HPP
#define CORE_SYSTEMS_CAMERAS_SHAKEFOLLOW_HPP

#include <Core/Systems/Cameras/Follow.hpp>

namespace core
{
namespace system
{
namespace camera
{
/**
 * @brief Special version of the Follow camera that also shakes
 *
 * @ingroup Cameras
 *
 */
class ShakeFollow : public Follow {
public:
    /// @brief Pointer to a ShakeFollow camera
    using Ptr = std::shared_ptr<ShakeFollow>;

    /**
     * @brief Creates a new ShakeFollow camera
     *
     * @param systems The main game systems
     * @param toFollow The entity to follow
     * @param shakesPerSec How many times to shake per second
     * @return Camera::Ptr The new camera
     */
    static Ptr create(Systems& systems, bl::entity::Entity toFollow, float shakesPerSec);

    /**
     * @brief Set the number of shakes per second
     *
     * @param shakesPerSec How many times to shake per second
     */
    void setShakesPerSecond(float shakesPerSec);

    /**
     * @brief Set the magnitude of the shaking
     *
     * @param mag The magnitude of the shaking, in pixels
     */
    void setMagnitude(float mag);

    /**
     * @brief Updates the camera to keep the view centered on the entity being followed while
     *        shaking around the entity
     *
     * @param systems The primary systems object
     * @param dt Time elapsed in seconds
     */
    virtual void update(Systems& systems, float dt) override;

private:
    float shakesPerSec;
    float magnitude;
    float time;

    ShakeFollow(Systems& systems, bl::entity::Entity entity, float sps);
};

} // namespace camera
} // namespace system
} // namespace core

#endif
