#ifndef CORE_SYSTEMS_FLIGHT_HPP
#define CORE_SYSTEMS_FLIGHT_HPP

#include <BLIB/Cameras/2D/Affectors/CameraShake.hpp>
#include <BLIB/Tilemap/Position.hpp>
#include <Core/Components/Renderable.hpp>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief Special system to manage player flight between their current location and a town
 *
 * @ingroup Systems
 *
 */
class Flight {
public:
    /**
     * @brief Construct a new Flight system
     *
     * @param systems The main game systems
     */
    Flight(Systems& systems);

    /**
     * @brief Returns whether or not the player is currently flying
     *
     */
    bool flying() const;

    /**
     * @brief Starts flight to the given spawn
     *
     * @param destSpawn The spawn to fly to
     * @return True if flight could start, false on error
     */
    bool startFlight(unsigned int destSpawn);

    /**
     * @brief Updates the player flight if in flight
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

private:
    enum struct State {
        Idle,
        Rising,
        Rotating,
        Accelerating,
        Flying,
        Deccelerating,
        UnRotating,
        Descending
    };

    struct RiseState {
        float height;
        float startY;
    };

    struct RotateState {
        float angle;
        float targetAngle;
        float rotateRate;
    };

    struct FlyState {
        float velocity;
        float angle;
        float ogDistSqrd;
        float maxSpeed;
    };

    Systems& owner;
    State state;
    bl::cam::c2d::CameraShake* cameraShake;
    bl::tmap::Position startPos;
    bl::tmap::Position* playerPos;
    component::Renderable* playerAnim;
    bl::tmap::Position destination;
    glm::vec2 flightDest;
    glm::vec2 unitVelocity;
    union {
        RiseState riseState;
        RotateState rotateState;
        FlyState flyState;
    };

    void movePlayer(float dt);
    void syncTiles();
};

} // namespace system
} // namespace core

#endif
