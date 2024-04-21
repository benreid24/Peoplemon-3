#ifndef GAME_STATES_EVOLUTION_PARTICLES_HPP
#define GAME_STATES_EVOLUTION_PARTICLES_HPP

#include <BLIB/Particles.hpp>
#include <Game/States/Evolution.hpp>
#include <glm/glm.hpp>

namespace game
{
namespace state
{
/// Implementation items for the evolution screen
namespace evo
{
/**
 * @brief GPU particle payload for the evolution sparks particle system
 *
 * @ingroup States
 */
struct GpuSpark {
    glm::vec2 pos;
    float radius;
    float alpha;

    GpuSpark() = default;

    GpuSpark& operator=(const Evolution::Spark& spark) {
        pos    = spark.position;
        radius = spark.radius;
        alpha  = 1.f - spark.time / spark.lifetime;
        return *this;
    }
};

/**
 * @brief Spark emitter for the evolution sparks particle system
 *
 * @ingroup States
 */
class SparkEmitter : public bl::pcl::Emitter<Evolution::Spark> {
public:
    SparkEmitter();
    virtual ~SparkEmitter() = default;

private:
    float residual;

    virtual void update(Emitter::Proxy& proxy, float dt, float) override;
};

/**
 * @brief Spark sink for the evolution sparks particle system
 *
 * @ingroup States
 */
class SparkSink : public bl::pcl::Sink<Evolution::Spark> {
public:
    virtual ~SparkSink() = default;

private:
    virtual void update(Proxy& proxy, std::span<Evolution::Spark> particles, float dt,
                        float) override;
};

/**
 * @brief Spark affector for the evolution sparks particle system
 *
 * @ingroup States
 */
class SparkAffector : public bl::pcl::Affector<Evolution::Spark> {
public:
    virtual ~SparkAffector() = default;

private:
    virtual void update(Proxy& proxy, float dt, float) override;
};

} // namespace evo
} // namespace state
} // namespace game

#endif
