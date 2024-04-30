#include <Game/States/Evolution/Particles.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <cmath>

namespace game
{
namespace state
{
namespace evo
{
namespace
{
constexpr unsigned int SparkCount = 400;
constexpr float SparkSpawnRate    = 1600.f;
const sf::Color SparkColor(252, 230, 30);
} // namespace

SparkEmitter::SparkEmitter()
: residual(0.f) {}

void SparkEmitter::update(Emitter::Proxy& proxy, float dt, float) {
    residual += SparkSpawnRate * dt;

    // TODO - cap spawn?
    while (residual >= 1.f) {
        residual -= 1.f;
        Evolution::Spark& sp = proxy.emit();

        const float a = bl::util::Random::get<float>(0.f, 360.f);
        const float d = bl::util::Random::get<float>(1.f, 40.f);
        const float v = bl::util::Random::get<float>(150.f, 900.f);
        const float c = bl::math::cos(a);
        const float s = bl::math::sin(a);
        sp.position   = glm::vec2(core::Properties::WindowSize().x * 0.5f,
                                core::Properties::WindowSize().y * 0.5f) +
                      glm::vec2(c, s) * d;
        sp.velocity.x = v * c;
        sp.velocity.y = v * s;
        sp.time       = 0.f;
        sp.lifetime   = bl::util::Random::get<float>(0.75f, 1.4f);
        sp.radius     = bl::util::Random::get<float>(2.f, 7.f);
    }
}

void SparkSink::update(Proxy& proxy, std::span<Evolution::Spark> particles, float dt, float) {
    for (auto& spark : particles) {
        spark.lifetime += dt;
        if (spark.time >= spark.lifetime) { proxy.destroy(spark); }
    }
}

void SparkAffector::update(Proxy& proxy, float dt, float) {
    for (auto& spark : proxy.particles()) { spark.position += spark.velocity * dt; }
}

} // namespace evo
} // namespace state
} // namespace game
