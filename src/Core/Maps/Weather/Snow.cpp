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
} // namespace

Snow::Snow(bool hard, bool thunder)
: fallSpeed(hard ? FastFallSpeed : SlowFallSpeed)
, snow(std::bind(&Snow::createFlake, this, std::placeholders::_1),
       hard ? Properties::HardSnowParticleCount() : Properties::LightSnowParticleCount(), 450.f)
, thunder(thunder, hard) {
    snowTxtr = bl::engine::Resources::textures().load(Properties::SnowFlakeFile()).data;
    snowFlake.setTexture(*snowTxtr, true);
    snowFlake.setOrigin(snowTxtr->getSize().x / 2, snowTxtr->getSize().y / 2);
    snow.setReplaceDestroyed(true);
}

void Snow::start(const sf::FloatRect& a) { area = a; }

void Snow::stop() { snow.setTargetCount(0); }

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
        else {
            flake.position.z -= dt;
        }

        return flake.position.z >= GroundTime;
    };

    snow.update(updateFlake, dt);
}

void Snow::render(sf::RenderTarget& target, float lag) const {
    const auto renderFlake = [this, &target, lag](const Flake& flake) {
        if (flake.position.z >= 0.f) {
            const sf::Vector3f interp(flake.position + flake.velocity * lag);
            this->snowFlake.setPosition(interp.x + interp.z / 4.f, interp.y - interp.z / 2.f);
            this->snowFlake.setColor(sf::Color(255, 255, 255, 255));
        }
        else {
            const float s = flake.position.z / GroundTime;
            this->snowFlake.setPosition(flake.position.x, flake.position.y);
            this->snowFlake.setColor(sf::Color(255, 255, 255, 255 - 255.f * s));
        }
        target.draw(this->snowFlake);
    };

    snow.render(renderFlake);

    area = {target.getView().getCenter() - target.getView().getSize() / 2.f,
            target.getView().getSize()};
}

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
