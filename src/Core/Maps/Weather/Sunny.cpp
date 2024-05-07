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
constexpr float Variance  = 55.f;
constexpr float BaseLine  = 145.f;
constexpr float StopDecay = 45.f;
constexpr float Wrap      = 2 * 3.141592653f / 3.f;

float computeScale(float t) { return (std::sin(3.f * t)) / 1.6f; }

int computeLevel(float t) { return BaseLine + computeScale(t) * Variance; }
} // namespace

Sunny::Sunny()
: t(0.f)
, stopping(false) {}

Weather::Type Sunny::type() const { return Weather::Sunny; }

void Sunny::start(bl::engine::Engine&) {
    t        = 0.f;
    stopping = false;
}

void Sunny::stop() {
    t        = computeLevel(t);
    stopping = true;
}

bool Sunny::stopped() const { return stopping && t <= 0.f; }

void Sunny::update(float dt) {
    if (stopping) { t -= StopDecay * dt; }
    else {
        t += dt;
        if (t >= Wrap) t = 0.f;
    }
}

// void Sunny::render(sf::RenderTarget& target, float lag) const {
//     const int a =
//         stopping ? std::max(0, static_cast<int>(t - StopDecay * lag)) : computeLevel(t + lag);
//     sun.setPosition(target.getView().getCenter());
//     sun.setSize(target.getView().getSize());
//     sun.setOrigin(target.getView().getSize() / 2.f);
//     sun.setFillColor(sf::Color(255, 200, 0, a));
//     target.draw(sun);
// }

} // namespace weather
} // namespace map
} // namespace core
