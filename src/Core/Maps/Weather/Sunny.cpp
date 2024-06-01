#include "Sunny.hpp"

#include <cmath>

namespace core
{
namespace map
{
namespace weather
{
namespace
{
constexpr float Variance = 35.f / 256.f;
constexpr float BaseLine = 165.f / 256.f;
constexpr float FadeRate = 1.f / 2.5f;
constexpr float Wrap     = 2.f * 3.141592653f / 3.f;
constexpr glm::vec3 Color(2.0f, 1.58f, 0.f);

float computeScale(float t) { return (std::sin(3.f * t)) / 1.6f; }

float computeLevel(float t) { return BaseLine + computeScale(t) * Variance; }

glm::vec3 computeColor(float t) {
    const float a = computeLevel(t);
    return {Color.x * a, Color.y * a, 0.f};
}

} // namespace

Sunny::Sunny()
: t(0.f)
, stopping(false)
, map(nullptr) {}

Weather::Type Sunny::type() const { return Weather::Sunny; }

void Sunny::start(bl::engine::Engine&, Map& m) {
    t        = 0.f;
    factor   = 0.f;
    stopping = false;
    map      = &m;
}

void Sunny::stop() { stopping = true; }

bool Sunny::stopped() const { return stopping && t <= 0.f; }

void Sunny::update(float dt) {
    t += dt;
    if (t >= Wrap) t = 0.f;

    if (stopping) { factor = std::max(0.f, factor - FadeRate * dt); }
    else { factor = std::min(1.f, factor + FadeRate * dt); }

    if (map) {
        const glm::vec3 color = computeColor(t) - glm::vec3(1.f);
        map->lightingSystem().setColorTint(glm::vec3(1.f) + color * factor);
    }
}

} // namespace weather
} // namespace map
} // namespace core
