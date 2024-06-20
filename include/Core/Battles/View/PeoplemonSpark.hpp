#ifndef CORE_BATTLES_VIEW_PEOPLEMONSPARK_HPP
#define CORE_BATTLES_VIEW_PEOPLEMONSPARK_HPP

#include <BLIB/Particles.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>
#include <glm/glm.hpp>

namespace core
{
namespace battle
{
namespace view
{
/**
 * @brief Particle system spark for peoplemon animations
 *
 * @ingroup Battles
 */
struct PeoplemonSpark {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    float time;
    float lifetime;

    PeoplemonSpark() = default;
};

/**
 * @brief Particle system spark GPU payload for peoplemon animations
 *
 * @ingroup Battles
 */
struct PeoplemonSparkGpu {
    glm::vec2 position;
    float radius;
    float lifetime;

    PeoplemonSparkGpu() = default;

    PeoplemonSparkGpu& operator=(const PeoplemonSpark& s) {
        position = s.position;
        radius   = s.radius;
        lifetime = s.time / s.lifetime;
        return *this;
    }
};

/**
 * @brief Global descriptor set payload for spark particle system renderer
 *
 * @ingroup Battles
 */
struct PeoplemonSparkGlobalShaderPayload {
    float maxAlphaReduction;
};

} // namespace view
} // namespace battle
} // namespace core

namespace bl
{
namespace pcl
{
using PeoplemonSpark     = core::battle::view::PeoplemonSpark;
using GpuPeoplemonSpark  = core::battle::view::PeoplemonSparkGpu;
using GlobalShaderStruct = core::battle::view::PeoplemonSparkGlobalShaderPayload;

template<>
struct RenderConfigMap<PeoplemonSpark> {
    static constexpr std::uint32_t PipelineId  = core::Properties::BattlePeoplemonSparkPipelineId;
    static constexpr bool ContainsTransparency = true;

    static constexpr bool CreateRenderPipeline = true;

    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        RenderConfigDefaults<PeoplemonSpark>::RenderPassIds;

    using GlobalShaderPayload = GlobalShaderStruct;
    using DescriptorSets =
        RenderConfigDescriptorList<rc::ds::Scene2DFactory,
                                   DescriptorSetFactory<PeoplemonSpark, GpuPeoplemonSpark>>;

    static constexpr bool EnableDepthTesting      = false;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    static constexpr const char* VertexShader = "Resources/Shaders/Particles/battleSpark.vert.spv";
    static constexpr const char* FragmentShader =
        "Resources/Shaders/Particles/battleSpark.frag.spv";
};
} // namespace pcl
} // namespace bl

namespace core
{
namespace battle
{
namespace view
{

/**
 * @brief Particle system spark emitter for explosions
 *
 * @ingroup Battle
 */
class SparkExplosionEmitter : public bl::pcl::Emitter<PeoplemonSpark> {
public:
    static constexpr float CreateRate      = 200.f;
    static constexpr unsigned int MaxSpawn = 50;

    SparkExplosionEmitter()
    : residual(0.f)
    , enabled(false) {}

    virtual ~SparkExplosionEmitter() = default;

    void setEnabled(bool e) { enabled = e; }

private:
    float residual;
    bool enabled;

    virtual void update(Proxy& proxy, float dt, float) override {
        constexpr float SquareSize = 200.f;

        if (enabled) {
            residual += CreateRate * dt;
            while (proxy.getManager().getParticleCount() < MaxSpawn && residual >= 1.f) {
                residual -= 1.f;
                auto& sp = proxy.emit();

                const float a = bl::util::Random::get<float>(0.f, 360.f);
                const float d = bl::util::Random::get<float>(1.f, 3.f);
                const float v = bl::util::Random::get<float>(150.f, 900.f);
                const float c = bl::math::cos(a);
                const float s = bl::math::sin(a);
                sp.position.x = SquareSize * 0.5f + d * c;
                sp.position.y = SquareSize * 0.5f + d * s;
                sp.velocity.x = v * c;
                sp.velocity.y = v * s;
                sp.time       = 0.f;
                sp.lifetime   = bl::util::Random::get<float>(0.6f, 1.1f);
                sp.radius     = bl::util::Random::get<float>(2.f, 12.f);
            }
        }
    }
};

/**
 * @brief Particle system spark emitter for implosions
 *
 * @ingroup Battle
 */
class SparkImplosionEmitter : public bl::pcl::Emitter<PeoplemonSpark> {
public:
    static constexpr float CreateRate      = 200.f;
    static constexpr unsigned int MaxSpawn = 50;

    SparkImplosionEmitter()
    : residual(0.f)
    , enabled(false) {}

    virtual ~SparkImplosionEmitter() = default;

    void setEnabled(bool e, const glm::vec2& origin = {}) {
        enabled       = e;
        implodeOrigin = origin;
    }

private:
    glm::vec2 implodeOrigin;
    float residual;
    bool enabled;

    virtual void update(Proxy& proxy, float dt, float) override {
        if (enabled) {
            residual += CreateRate * dt;
            while (proxy.getManager().getParticleCount() < MaxSpawn && residual >= 1.f) {
                residual -= 1.f;
                auto& sp = proxy.emit();

                const float a = bl::util::Random::get<float>(0.f, 360.f);
                const float d = bl::util::Random::get<float>(190.f, 210.f);
                const float v = bl::util::Random::get<float>(450.f, 900.f);
                const float c = bl::math::cos(a);
                const float s = bl::math::sin(a);
                sp.position.x = implodeOrigin.x + d * c;
                sp.position.y = implodeOrigin.y + d * s;
                sp.velocity.x = v * -c;
                sp.velocity.y = v * -s;
                sp.time       = 0.f;
                sp.lifetime   = d / v;
                sp.radius     = bl::util::Random::get<float>(2.f, 12.f);
            }
        }
    }
};

/**
 * @brief Particle system spark affector
 *
 * @ingroup Battles
 */
class SparkAffector : public bl::pcl::Affector<PeoplemonSpark> {
public:
    virtual ~SparkAffector() = default;

private:
    virtual void update(Proxy& proxy, float dt, float) override {
        for (auto& spark : proxy.particles()) {
            spark.position += spark.velocity * dt;
            spark.time += dt;
        }
    }
};

/**
 * @brief Particle system spark sink
 *
 * @ingroup Battles
 */
class SparkSink : public bl::pcl::Sink<PeoplemonSpark> {
public:
    virtual ~SparkSink() = default;

private:
    virtual void update(Proxy& proxy, std::span<PeoplemonSpark> particles, float, float) override {
        for (PeoplemonSpark& spark : particles) {
            if (spark.time >= spark.lifetime) { proxy.destroy(spark); }
        }
    }
};

} // namespace view
} // namespace battle
} // namespace core

#endif
