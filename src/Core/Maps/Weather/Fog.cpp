#include "Fog.hpp"

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
constexpr float AlphaRate = 25.f;
const sf::Vector2f FogVelocity(64.f, 25.f);

float computeAlpha(float current, std::uint8_t target, float step) {
    const float rate = target > current ? AlphaRate : -AlphaRate;
    const float na   = current + rate * step;
    if (na < 0) return 0;
    if (na > 255) return 255;
    if (current < target && na > target) return target;
    return na;
}

} // namespace

Fog::Fog(bool thick)
: maxOpacity(thick ? Properties::ThickFogAlpha() : Properties::ThinFogAlpha())
, targetOpacity(0)
, alpha(0) {
    fogTxtr = bl::engine::Resources::textures().load(Properties::FogFile()).data;
    fog.setTexture(*fogTxtr, true);
    fog.setOrigin(fogTxtr->getSize().x / 2, fogTxtr->getSize().y / 2);
}

Weather::Type Fog::type() const {
    return maxOpacity == Properties::ThickFogAlpha() ? Weather::ThickFog : Weather::ThinFog;
}

void Fog::start(const sf::FloatRect& a) {
    area                      = a;
    const unsigned int width  = area.width * 2 / (fogTxtr->getSize().x / 2);
    const unsigned int height = area.height * 2 / (fogTxtr->getSize().y / 2);
    const sf::Vector2f corner(area.left - area.width / 2.f, area.top - area.height / 2.f);

    particles.clear();
    particles.reserve(width * height);
    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            particles.emplace_back();
            particles.back().set({static_cast<float>(x * fogTxtr->getSize().x / 2),
                                  static_cast<float>(y * fogTxtr->getSize().y / 2)});
        }
    }

    alpha         = 0;
    targetOpacity = maxOpacity;
}

void Fog::stop() { targetOpacity = 0; }

bool Fog::stopped() const { return alpha == 0; }

void Fog::update(float dt) {
    if (alpha != targetOpacity) { alpha = computeAlpha(alpha, targetOpacity, dt); }
    for (Particle& p : particles) {
        p.position += FogVelocity * dt;
        p.rotation += p.angularVelocity * dt;
        if (p.position.x >= area.left + area.width * 1.5f) { p.position.x -= area.width * 2.f; }
        else if (p.position.x <= area.left - area.width / 2.f) {
            p.position.x += area.width * 2.f;
        }
        if (p.position.y >= area.top + area.height * 1.5f) { p.position.y -= area.height * 2.f; }
        else if (p.position.y <= area.top - area.height / 2.f) {
            p.position.y += area.height * 2.f;
        }
    }
}

void Fog::render(sf::RenderTarget& target, float lag) const {
    area = {target.getView().getCenter() - target.getView().getSize() / 2.f,
            target.getView().getSize()};

    fog.setColor(sf::Color(255, 255, 255, computeAlpha(alpha, targetOpacity, lag)));
    for (const Particle& p : particles) {
        fog.setPosition(p.position + FogVelocity * lag);
        fog.setRotation(p.rotation + p.angularVelocity * lag);
        fog.setScale(p.scale, p.scale);
        target.draw(fog);
    }
}

void Fog::Particle::set(const sf::Vector2f& pos) {
    position        = pos;
    rotation        = bl::util::Random::get<float>(0.f, 360.f);
    angularVelocity = bl::util::Random::get<float>(-180.f, 180.f);
    scale           = bl::util::Random::get<float>(0.8f, 1.8f);
}

} // namespace weather
} // namespace map
} // namespace core
