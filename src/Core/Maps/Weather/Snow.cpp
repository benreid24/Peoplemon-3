#include "Snow.hpp"

#include <BLIB/Cameras/2D/Camera2D.hpp>
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
constexpr float SpawnRate       = 450.f;
constexpr float FastFallSpeed   = -340.f;
constexpr float SlowFallSpeed   = -100.f;
constexpr float GroundTime      = -2.f;
constexpr float HorizontalAccel = 128.f;
constexpr float HorizontalVel   = 64.f;
constexpr float StopTime        = 4.f;
constexpr float StopSpeed       = 1.f / StopTime;
constexpr float RespawnDelay    = 2.f;

std::size_t deadCount = 0;
} // namespace

namespace snow
{
struct Snowflake {
    glm::vec2 pos;
    glm::vec2 vel;
    float height;

    Snowflake() = default;

    Snowflake(const sf::FloatRect& area) {
        pos.x  = bl::util::Random::get<float>(area.left - 300.f, area.left + area.width + 300.f);
        pos.y  = bl::util::Random::get<float>(area.top - 300.f, area.top + area.height + 300.f);
        vel.x  = bl::util::Random::get<float>(-HorizontalVel, HorizontalVel);
        vel.y  = bl::util::Random::get<float>(-1.f, 1.f);
        height = bl::util::Random::get<float>(140.f, 200.f);
    }
};

struct GpuSnowflake {
    glm::vec2 pos;
    float height;
    float padding;

    GpuSnowflake() = default;

    GpuSnowflake& operator=(const Snowflake& drop) {
        pos    = drop.pos;
        height = drop.height;
        return *this;
    }
};

struct alignas(8) GlobalShaderInfo {
    glm::vec2 textureCenter;
    std::uint32_t textureId;
    float radius;
};
} // namespace snow
} // namespace weather
} // namespace map
} // namespace core

namespace bl
{
namespace pcl
{
using Snowflake        = core::map::weather::snow::Snowflake;
using GpuSnowflake     = core::map::weather::snow::GpuSnowflake;
using GlobalShaderInfo = core::map::weather::snow::GlobalShaderInfo;

template<>
struct RenderConfigMap<Snowflake> {
    static constexpr std::uint32_t PipelineId  = core::Properties::SnowflakePipelineId;
    static constexpr bool ContainsTransparency = true;

    static constexpr bool CreateRenderPipeline = true;

    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        bl::pcl::RenderConfigDefaults<Snowflake>::RenderPassIds;

    using GlobalShaderPayload = GlobalShaderInfo;
    using DescriptorSets =
        bl::pcl::RenderConfigDescriptorList<bl::rc::ds::TexturePoolFactory,
                                            bl::rc::ds::Scene2DFactory,
                                            bl::pcl::DescriptorSetFactory<Snowflake, GpuSnowflake>>;

    static constexpr bool EnableDepthTesting      = true;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    static constexpr const char* VertexShader = "Resources/Shaders/Particles/snowflake.vert.spv";
    static constexpr const char* FragmentShader =
        bl::rc::Config::ShaderIds::Fragment2DRotatedParticle;
};
} // namespace pcl
} // namespace bl

namespace core
{
namespace map
{
namespace weather
{
namespace snow
{
class GravityAffector : public bl::pcl::Affector<Snowflake> {
public:
    GravityAffector(float fallVel)
    : fallVel(fallVel) {}

    virtual ~GravityAffector() = default;

    virtual void update(Proxy& proxy, float dt, float) override {
        for (Snowflake& flake : proxy.particles()) {
            if (flake.height > 0.f) {
                flake.pos += flake.vel * dt;
                flake.height += fallVel * dt;
                if (flake.vel.y > 0.f) {
                    flake.vel.x -= HorizontalAccel * dt;
                    if (flake.vel.x < -HorizontalVel) { flake.vel.y = -1.f; }
                }
                else {
                    flake.vel.x += HorizontalAccel * dt;
                    if (flake.vel.x > HorizontalVel) { flake.vel.y = 1.f; }
                }

                if (flake.height < 0.f) flake.height = -0.001f;
            }
            else { flake.height -= dt; }
        }
    }

private:
    const float fallVel;
};

class TimeSink : public bl::pcl::Sink<Snowflake> {
public:
    TimeSink()
    : time(0.f) {}

    virtual ~TimeSink() = default;

    virtual void update(Proxy& proxy, std::span<Snowflake> particles, float dt, float) override {
        time += dt;
        for (Snowflake& drop : particles) {
            if (drop.height <= GroundTime) {
                proxy.destroy(drop);
                if (time >= RespawnDelay) { ++deadCount; }
            }
        }
    }

private:
    float time;
};

class TimeEmitter : public bl::pcl::Emitter<Snowflake> {
public:
    TimeEmitter(bl::rc::RenderTarget& observer, std::size_t target)
    : observer(observer)
    , target(target)
    , residual(0.f) {}

    virtual ~TimeEmitter() = default;

    void setTarget(std::size_t t) { target = t; }

    virtual void update(Proxy& proxy, float dt, float) override {
        const std::size_t current = proxy.getManager().getParticleCount();
        if (current < target) {
            residual += SpawnRate * dt;
            if (residual >= 1.f || deadCount > 0) {
                const std::size_t maxSpawn = target - current;
                const std::size_t toSpawn =
                    std::min(static_cast<std::size_t>(std::floor(residual)) + deadCount, maxSpawn);
                residual -= static_cast<float>(toSpawn);

                bl::cam::Camera2D* cam = observer.getCurrentCamera<bl::cam::Camera2D>();
                if (!cam) { return; }
                const sf::FloatRect area = cam->getVisibleArea();
                for (std::size_t i = 0; i < toSpawn; ++i) { proxy.emit(area); }
            }
        }
    }

private:
    bl::rc::RenderTarget& observer;
    std::size_t target;
    float residual;
};

} // namespace snow

Snow::Snow(bool hard, bool thunder)
: engine(nullptr)
, _type(hard ? (thunder ? Weather::HardSnowThunder : Weather::HardSnow) :
               (thunder ? Weather::LightSnowThunder : Weather::LightSnow))
, targetParticleCount(hard ? Properties::HardSnowParticleCount() :
                             Properties::LightSnowParticleCount())
, fallSpeed(hard ? FastFallSpeed : SlowFallSpeed)
, stopFactor(-1.f)
, thunder(thunder, hard) {}

Snow::~Snow() {
    stop();
    if (engine) { engine->particleSystem().removeUniqueSystem<snow::Snowflake>(); }
}

Weather::Type Snow::type() const { return _type; }

void Snow::start(bl::engine::Engine& e, bl::rc::RenderTarget& renderTarget, Map& map) {
    engine    = &e;
    deadCount = 0;

    const auto sampler = e.renderer().vulkanState().samplerCache.noFilterBorderClamped();
    snowTxtr = e.renderer().texturePool().getOrLoadTexture(Properties::SnowFlakeFile(), sampler);

    particles = &e.particleSystem().getUniqueSystem<snow::Snowflake>();

    particles->getRenderer().getGlobals().textureId = snowTxtr.id();
    particles->getRenderer().getGlobals().textureCenter =
        snowTxtr->normalizeAndConvertCoord(snowTxtr->size() * 0.5f);
    particles->getRenderer().getGlobals().radius = glm::length(snowTxtr->size()) * 0.5f;

    emitter = particles->addEmitter<snow::TimeEmitter>(renderTarget, targetParticleCount);
    particles->addAffector<snow::GravityAffector>(fallSpeed);
    particles->addSink<snow::TimeSink>();
    particles->addToScene(renderTarget.getCurrentScene());
    particles->getRenderer().getComponent()->vertexBuffer.vertices()[0].pos.z = map.getMinDepth();
}

void Snow::stop() {
    stopFactor = 0.f;
    thunder.stop();
}

bool Snow::stopped() const { return particles && particles->getParticleCount() == 0; }

void Snow::update(float dt) {
    if (stopFactor >= 0.f) {
        stopFactor = std::min(stopFactor + StopSpeed * dt, 1.f);
        emitter->setTarget(
            targetParticleCount -
            static_cast<unsigned int>(static_cast<float>(targetParticleCount) * stopFactor));
        if (stopFactor >= 0.8f) {
            stopFactor = -1.f;
            emitter->setTarget(0);
        }
    }
}

} // namespace weather
} // namespace map
} // namespace core
