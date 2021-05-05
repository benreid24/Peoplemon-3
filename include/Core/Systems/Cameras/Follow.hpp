#ifndef CORE_SYSTEMS_CAMERAS_FOLLOW_HPP
#define CORE_SYSTEMS_CAMERAS_FOLLOW_HPP

#include <Core/Systems/Cameras/Camera.hpp>

#include <BLIB/Entities.hpp>
#include <BLIB/Events.hpp>
#include <Core/Components/Position.hpp>

namespace core
{
namespace system
{
class Systems;

namespace camera
{
/**
 * @brief Camera that follows a given entity
 *
 * @ingroup Cameras
 *
 */
class Follow
: public Camera
, public bl::event::Listener<bl::entity::event::EntityDestroyed> {
public:
    /**
     * @brief Creates a new follow camera
     *
     * @param systems The primary systems object
     * @param toFollow The entity to follow
     * @return Camera::Ptr The new camera
     */
    static Camera::Ptr create(Systems& systems, bl::entity::Entity toFollow);

    /**
     * @brief Destroy the Follow camera
     *
     */
    virtual ~Follow() = default;

    /**
     * @brief Returns true if the entity being followed is still alive, false otherwise
     *
     */
    virtual bool valid() const override;

    /**
     * @brief Updates the camera to keep the view centered on the entity being followed
     *
     * @param systems The primary systems object
     * @param dt Time elapsed in seconds
     */
    virtual void update(Systems& systems, float dt) override;

private:
    const bl::entity::Entity owner;
    bl::entity::Registry::ComponentHandle<component::Position> position;
    bool alive;

    Follow(Systems& systems, bl::entity::Entity toFollow);
    virtual void observe(const bl::entity::event::EntityDestroyed& event) override;
};

} // namespace camera
} // namespace system
} // namespace core

#endif
