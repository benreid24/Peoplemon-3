#include "Snow.hpp"

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace map
{
namespace weather
{
namespace
{
constexpr float FastFallSpeed   = -340.f;
constexpr float SlowFallSpeed   = -100.f;
constexpr float GroundTime      = -2.f;
constexpr float HorizontalAccel = 128.f;
constexpr float HorizontalVel   = 64.f;
constexpr float StopTime        = 4.f;
constexpr float StopSpeed       = 1.f / StopTime;
} // namespace

Snow::Snow(bool hard, bool thunder)
: _type(hard ? (thunder ? Weather::HardSnowThunder : Weather::HardSnow) :
               (thunder ? Weather::LightSnowThunder : Weather::LightSnow))
, targetParticleCount(hard ? Properties::HardSnowParticleCount() :
                             Properties::LightSnowParticleCount())
, fallSpeed(hard ? FastFallSpeed : SlowFallSpeed)
, stopFactor(-1.f)
, snow(std::bind(&Snow::createFlake, this, std::placeholders::_1), targetParticleCount, 450.f)
, thunder(thunder, hard) {
    snowTxtr = TextureManager::load(Properties::SnowFlakeFile());
    snowFlake.setTexture(*snowTxtr, true);
    snowFlake.setOrigin(snowTxtr->getSize().x / 2, snowTxtr->getSize().y / 2);
    snow.setReplaceDestroyed(true);
}

Weather::Type Snow::type() const { return _type; }

void Snow::start(bl::engine::Engine&) {
    // TODO
}

void Snow::stop() {
    stopFactor = 0.f;
    thunder.stop();
}

bool Snow::stopped() const { return snow.particleCount() == 0; }

void Snow::update(float dt) {
    const auto updateFlake = [this, dt](Flake& flake) -> bool {
        if (flake.position.z > 0.f) {
            flake.position += flake.velocity * dt;
            if (flake.velocity.y > 0.f) {
                flake.velocity.x -= HorizontalAccel * dt;
                if (flake.velocity.x < -HorizontalVel) { flake.velocity.y = -1.f; }
            }
            else {
                flake.velocity.x += HorizontalAccel * dt;
                if (flake.velocity.x > HorizontalVel) { flake.velocity.y = 1.f; }
            }

            if (flake.position.z < 0.f) flake.position.z = -0.001f;
        }
        else { flake.position.z -= dt; }

        return flake.position.z >= GroundTime;
    };

    snow.update(updateFlake, dt);

    if (stopFactor >= 0.f) {
        stopFactor = std::min(stopFactor + StopSpeed * dt, 1.f);
        snow.setTargetCount(
            targetParticleCount -
            static_cast<unsigned int>(static_cast<float>(targetParticleCount) * stopFactor));
        if (stopFactor >= 0.8f) {
            stopFactor = -1.f;
            snow.setTargetCount(0);
        }
    }
}

// void Snow::render(sf::RenderTarget& target, float lag) const {
//     const auto renderFlake = [this, &target, lag](const Flake& flake) {
//         if (flake.position.z >= 0.f) {
//             const sf::Vector3f interp(flake.position + flake.velocity * lag);
//             this->snowFlake.setPosition(interp.x + interp.z / 4.f, interp.y - interp.z / 2.f);
//             this->snowFlake.setColor(sf::Color(255, 255, 255, 255));
//         }
//         else {
//             const float s = flake.position.z / GroundTime;
//             this->snowFlake.setPosition(flake.position.x, flake.position.y);
//             this->snowFlake.setColor(sf::Color(255, 255, 255, 255 - 255.f * s));
//         }
//         target.draw(this->snowFlake);
//     };
//
//     snow.render(renderFlake);
//
//     area = {target.getView().getCenter() - target.getView().getSize() / 2.f,
//             target.getView().getSize()};
// }

void Snow::createFlake(Flake* flake) {
    flake = new (flake)
        Flake(bl::util::Random::get<float>(area.left - 300.f, area.left + area.width + 300.f),
              bl::util::Random::get<float>(area.top - 300.f, area.top + area.height + 300.f),
              bl::util::Random::get<float>(140.f, 200.f),
              fallSpeed);
}

Snow::Flake::Flake(float x, float y, float z, float zf)
: position(x, y, z)
, velocity(bl::util::Random::get<float>(-HorizontalVel, HorizontalVel),
           bl::util::Random::get<float>(-1.f, 1.f), zf) {}

} // namespace weather
} // namespace map
} // namespace core
