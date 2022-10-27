#include <Core/Systems/Flight.hpp>

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
constexpr float RiseHeight     = 32.5 * 5.f;
constexpr float RiseRate       = RiseHeight / 2.f;
constexpr float RotateTime     = 0.65f;
constexpr float MaxSpeed       = 32.f * 38.f;
constexpr float AccelRate      = MaxSpeed / 2.5f;
constexpr float DeccelDist     = MaxSpeed * MaxSpeed / (2 * AccelRate);
constexpr float DeccelDistSqrd = DeccelDist * DeccelDist;

float distSqrd(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    const float dx = p1.x - p2.x;
    const float dy = p1.y - p2.y;
    return dx * dx + dy * dy;
}

float distSqrd(const component::Position& p1, const sf::Vector2f& p2) {
    return distSqrd(p1.positionPixels(), p2);
}

} // namespace

Flight::Flight(Systems& s)
: owner(s)
, state(State::Idle)
, playerPos(nullptr)
, playerAnim(nullptr) {}

bool Flight::flying() const { return state != State::Idle; }

bool Flight::startFlight(unsigned int spawn) {
    if (!owner.world().activeMap().canFlyFromHere()) {
        BL_LOG_WARN << "Tried to fly from non-flyable map";
        return false;
    }

    const component::Position* spos = owner.world().activeMap().getSpawnPosition(spawn);
    if (!spos) {
        BL_LOG_ERROR << "Tried to fly to bad spawn: " << spawn;
        return false;
    }
    destination = *spos;

    playerPos =
        owner.engine().entities().getComponent<component::Position>(owner.player().player());
    playerAnim =
        owner.engine().entities().getComponent<component::Renderable>(owner.player().player());
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

    if (playerPos->positionTiles() == destination.positionTiles()) {
        // options:
        // 1. cancel with message
        // 2. go up and down
        // 3. go up and down with message
    }

    // remove player control and face down
    owner.player().removePlayerControlled(owner.player().player());
    startPos             = *playerPos;
    playerPos->direction = component::Direction::Down;
    playerPos->level     = std::max(playerPos->level, destination.level);

    state            = State::Rising;
    riseState.height = 0.f;
    riseState.startY = playerPos->positionPixels().y;

    return true;
}

void Flight::update(float dt) {
    switch (state) {
    case State::Rising:
        riseState.height = std::min(riseState.height + RiseRate * dt, RiseHeight);
        playerPos->setPixels({playerPos->positionPixels().x, riseState.startY - riseState.height});
        if (riseState.height >= RiseHeight) {
            state      = State::Rotating;
            flightDest = destination.positionPixels();
            flightDest.y -= RiseHeight;
            rotateState.angle       = 0.f;
            rotateState.targetAngle = bl::math::radiansToDegrees(
                std::atan2(flightDest.y - playerPos->positionPixels().y,
                           flightDest.x - playerPos->positionPixels().x));
            rotateState.rotateRate = (rotateState.targetAngle - rotateState.angle) / RotateTime;
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
            }
            else {
                state                     = State::Descending;
                riseState.height          = RiseHeight;
                const sf::Vector2f pixels = playerPos->positionPixels();
                playerPos->setTiles(destination.positionTiles());
                playerPos->setPixels(pixels);
            }
        }
        break;

    case State::Accelerating:
        flyState.velocity = std::min(flyState.velocity + AccelRate * dt, MaxSpeed);
        movePlayer(dt);
        if (flyState.velocity == MaxSpeed) { state = State::Flying; }
        else if (distSqrd(*playerPos, flightDest) <= flyState.ogDistSqrd * 0.2f) {
            // TODO - wtf?
            state = State::Deccelerating;
        }
        break;

    case State::Flying:
        movePlayer(dt);
        if (distSqrd(*playerPos, flightDest) <= DeccelDistSqrd) { state = State::Deccelerating; }
        break;

    case State::Deccelerating:
        flyState.velocity = std::max(flyState.velocity - AccelRate * dt, 0.f);
        movePlayer(dt);
        if (distSqrd(*playerPos, flightDest) <= 3.4f || flyState.velocity == 0.f) {
            playerPos->setPixels(flightDest);
            const float pa          = flyState.angle;
            state                   = State::UnRotating;
            rotateState.angle       = pa;
            rotateState.targetAngle = 0.f;
            rotateState.rotateRate  = (rotateState.targetAngle - rotateState.angle) / RotateTime;
        }
        break;

    case State::Descending:
        riseState.height = std::max(riseState.height - RiseRate * dt, 0.f);
        playerPos->setPixels(
            {playerPos->positionPixels().x, destination.positionPixels().y - riseState.height});
        if (riseState.height == 0.f) {
            *playerPos           = destination;
            playerPos->direction = component::Direction::Down;
            state                = State::Idle;
            owner.engine().eventBus().dispatch<event::EntityMoved>(
                {owner.player().player(), *playerPos, *playerPos});
            owner.player().makePlayerControlled(owner.player().player());
        }
        break;

    case State::Idle:
    default:
        break;
    }
}

void Flight::movePlayer(float dt) {
    const sf::Vector2f vel(bl::math::cos(flyState.angle), bl::math::sin(flyState.angle));
    playerPos->setPixels(playerPos->positionPixels() + vel * flyState.velocity * dt);
    syncTiles();
}

void Flight::syncTiles() {
    const sf::Vector2f pixels = playerPos->positionPixels();
    sf::Vector2i tiles(pixels);
    tiles.y += static_cast<int>(RiseHeight);
    tiles /= Properties::PixelsPerTile();
    if (tiles != playerPos->positionTiles()) {
        const auto old = *playerPos;
        playerPos->setTiles(tiles);
        playerPos->setPixels(pixels);
        owner.engine().eventBus().dispatch<event::EntityMoved>(
            {owner.player().player(), old, *playerPos});
    }
}

} // namespace system
} // namespace core
