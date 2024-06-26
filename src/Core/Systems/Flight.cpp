#include <Core/Systems/Flight.hpp>

#include <BLIB/Cameras.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Math.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>
#include <cmath>

namespace core
{
namespace system
{
namespace
{
constexpr float RiseHeight      = 32.5 * 5.f;
constexpr float RiseRate        = RiseHeight / 2.f;
constexpr float RotateTime      = 0.65f;
constexpr float MaxSpeed        = 32.f * 38.f;
constexpr float MinFlyTime      = 2.5f;
constexpr float ShakesPerSec    = 18.f;
constexpr float ShakeMagnitude  = 7.f;
constexpr float ShadowSize      = 17.f;
constexpr float ShadowShrinkage = ShadowSize * 0.3f;
constexpr float HoldTime        = 1.5f;

float distSqrd(const glm::vec2& p1, const glm::vec2& p2) {
    const float dx = p1.x - p2.x;
    const float dy = p1.y - p2.y;
    return dx * dx + dy * dy;
}

float distSqrd(const bl::tmap::Position& p1, const glm::vec2& p2) {
    return distSqrd(p1.transform->getGlobalPosition(), p2);
}

} // namespace

Flight::Flight(Systems& s)
: owner(s)
, state(State::Idle)
, cameraShake(nullptr)
, playerPos(nullptr)
, playerAnim(nullptr) {}

void Flight::init(bl::engine::Engine&) {}

bool Flight::flying() const { return state != State::Idle; }

bool Flight::startFlight(unsigned int spawn) {
    if (!owner.world().activeMap().canFlyFromHere()) {
        BL_LOG_WARN << "Tried to fly from non-flyable map";
        return false;
    }

    const bl::tmap::Position* spos = owner.world().activeMap().getSpawnPosition(spawn);
    if (!spos) {
        BL_LOG_ERROR << "Tried to fly to bad spawn: " << spawn;
        return false;
    }
    destination = *spos;

    playerPos  = owner.engine().ecs().getComponent<bl::tmap::Position>(owner.player().player());
    playerAnim = owner.engine().ecs().getComponent<component::Renderable>(owner.player().player());
    if (!playerPos) {
        BL_LOG_CRITICAL << "Player position could not be found";
        owner.engine().flags().set(bl::engine::Flags::Terminate);
        return false;
    }
    if (!playerAnim) {
        BL_LOG_CRITICAL << "Player animation could not be found";
        owner.engine().flags().set(bl::engine::Flags::Terminate);
        return false;
    }

    // remove player control and face down
    owner.player().removePlayerControlled(owner.player().player());
    startPos             = *playerPos;
    playerPos->direction = bl::tmap::Direction::Down;
    playerPos->level     = std::max(playerPos->level, destination.level);
    playerPos->transform->setDepth(owner.world().activeMap().getMinDepth());
    rotatePlayer(bl::tmap::Direction::Down);

    // start flight
    state            = State::Rising;
    riseState.height = 0.f;
    riseState.startY = playerPos->transform->getGlobalPosition().y;

    return true;
}

void Flight::update(std::mutex&, float dt, float, float, float) {
    switch (state) {
    case State::Rising:
        riseState.height = std::min(riseState.height + RiseRate * dt, RiseHeight);
        playerPos->transform->setPosition(playerPos->transform->getGlobalPosition().x,
                                          riseState.startY - riseState.height);
        owner.render().updateShadow(owner.player().player(),
                                    riseState.height,
                                    ShadowSize - (riseState.height / RiseHeight) * ShadowShrinkage);
        if (riseState.height >= RiseHeight) {
            if (startPos.position != destination.position) {
                state      = State::Rotating;
                flightDest = destination.getWorldPosition(Properties::PixelsPerTile());
                flightDest.y -= RiseHeight;
                unitVelocity = flightDest - playerPos->transform->getGlobalPosition();
                const float m =
                    std::sqrt(unitVelocity.x * unitVelocity.x + unitVelocity.y * unitVelocity.y);
                unitVelocity.x /= m;
                unitVelocity.y /= m;
                rotateState.angle = 0.f;
                rotateState.targetAngle =
                    bl::math::radiansToDegrees(
                        std::atan2(flightDest.y - playerPos->transform->getGlobalPosition().y,
                                   flightDest.x - playerPos->transform->getGlobalPosition().x)) +
                    90.f;
                rotateState.rotateRate = (rotateState.targetAngle - rotateState.angle) / RotateTime;
            }
            else {
                state              = State::Holding;
                holdState.lookTime = 0.f;
            }
        }
        break;

    case State::Rotating:
    case State::UnRotating:
        rotateState.angle += rotateState.rotateRate * dt;
        playerAnim->setAngle(rotateState.angle);
        if (std::abs(rotateState.angle - rotateState.targetAngle) < 1.5f) {
            rotateState.angle = rotateState.targetAngle;
            playerAnim->setAngle(rotateState.angle);

            if (state == State::Rotating) {
                const float pa      = rotateState.targetAngle;
                state               = State::Accelerating;
                flyState.angle      = pa; // probably not required but eh
                flyState.velocity   = 0.f;
                flyState.ogDistSqrd = distSqrd(*playerPos, flightDest);
                flyState.maxSpeed = std::min(std::sqrt(flyState.ogDistSqrd) / MinFlyTime, MaxSpeed);

                cameraShake = owner.engine()
                                  .renderer()
                                  .getObserver()
                                  .getCurrentCamera<bl::cam::Camera2D>()
                                  ->addAffector<bl::cam::c2d::CameraShake>(0.f, 0.f);
            }
            else {
                state            = State::Descending;
                riseState.height = RiseHeight;
            }
        }
        break;

    case State::Accelerating: {
        movePlayer(dt);
        const float cDist   = distSqrd(*playerPos, flightDest);
        const float percent = 1.f - (cDist / flyState.ogDistSqrd) + 0.005f;
        flyState.velocity =
            percent <= 0.125f ? percent * 8.f * flyState.maxSpeed : flyState.maxSpeed;
        if (flyState.velocity >= flyState.maxSpeed) {
            flyState.velocity = flyState.maxSpeed;
            state             = State::Flying;
        }
        if (cameraShake) {
            cameraShake->setMagnitude(flyState.velocity / flyState.maxSpeed * ShakeMagnitude);
            cameraShake->setShakesPerSecond(flyState.velocity / flyState.maxSpeed * ShakesPerSec);
        }
    } break;

    case State::Flying: {
        movePlayer(dt);
        const float cDist   = distSqrd(*playerPos, flightDest);
        const float percent = 1.f - cDist / flyState.ogDistSqrd;
        if (percent >= 0.98f) { state = State::Deccelerating; }
    } break;

    case State::Deccelerating: {
        movePlayer(dt);
        const float cDist   = distSqrd(*playerPos, flightDest);
        const float percent = 1.f - cDist / flyState.ogDistSqrd;
        flyState.velocity   = std::max((1.f - percent) * 50.f * flyState.maxSpeed, 32.f * 6.f);
        if (cDist <= 3.4f || flyState.velocity == 0.f) {
            if (cameraShake) { cameraShake->setMagnitude(0.f); }
            playerPos->transform->setPosition(flightDest);
            const float pa          = flyState.angle;
            state                   = State::UnRotating;
            rotateState.angle       = pa;
            rotateState.targetAngle = 0.f;
            rotateState.rotateRate  = (rotateState.targetAngle - rotateState.angle) / RotateTime;

            owner.engine()
                .renderer()
                .getObserver()
                .getCurrentCamera<bl::cam::Camera2D>()
                ->removeAffector(cameraShake);
            cameraShake = nullptr;
        }
        else if (cameraShake) {
            cameraShake->setMagnitude(flyState.velocity / flyState.maxSpeed * ShakeMagnitude);
            cameraShake->setShakesPerSecond(flyState.velocity / flyState.maxSpeed * ShakesPerSec);
        }
    } break;

    case State::Descending:
        riseState.height = std::max(riseState.height - RiseRate * dt, 0.f);
        playerPos->transform->setPosition(
            playerPos->transform->getGlobalPosition().x,
            destination.getWorldPosition(Properties::PixelsPerTile()).y - riseState.height);
        owner.render().updateShadow(owner.player().player(),
                                    riseState.height,
                                    ShadowSize - (riseState.height / RiseHeight) * ShadowShrinkage);
        if (riseState.height == 0.f) {
            owner.render().removeShadow(owner.player().player());
            playerPos->position = destination.position;
            state               = State::Idle;
            owner.player().makePlayerControlled(owner.player().player());
            if (startPos.position != destination.position) {
                playerPos->syncTransform(Properties::PixelsPerTile());
                bl::event::Dispatcher::dispatch<event::EntityMoved>(
                    {owner.player().player(), startPos, *playerPos});
                bl::event::Dispatcher::dispatch<event::EntityMoveFinished>(
                    {owner.player().player(), *playerPos});
            }
            else {
                owner.hud().displayMessage("...");
                owner.hud().displayMessage("We're here");
            }
        }
        break;

    case State::Holding:
        holdState.lookTime += dt;
        if (holdState.lookTime >= HoldTime) {
            switch (playerPos->direction) {
            case bl::tmap::Direction::Down:
                rotatePlayer(bl::tmap::Direction::Right);
                holdState.lookTime = 0.f;
                break;
            case bl::tmap::Direction::Right:
                rotatePlayer(bl::tmap::Direction::Left);
                holdState.lookTime = 0.f;
                break;
            case bl::tmap::Direction::Left:
            default:
                rotatePlayer(bl::tmap::Direction::Down);
                state            = State::Descending;
                riseState.height = RiseHeight;
                break;
            }
        }
        break;

    case State::Idle:
    default:
        break;
    }
}

void Flight::movePlayer(float dt) {
    playerPos->transform->move(unitVelocity * flyState.velocity * dt);
}

void Flight::rotatePlayer(bl::tmap::Direction dir) {
    bl::event::Dispatcher::dispatch<event::EntityRotated>(
        {owner.player().player(), dir, playerPos->direction});
    playerPos->direction = dir;
}

} // namespace system
} // namespace core
