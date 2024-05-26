#include "Rain.hpp"

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Particles.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace
{
constexpr float SpawnRate  = 500.f;
constexpr float SplashTime = -0.15f;
constexpr float TransTime  = -0.3f;
constexpr float DeadTime   = -0.4f;
constexpr float StopTime   = 4.f;
constexpr float StopSpeed  = 1.f / StopTime;
} // namespace

namespace core
{
namespace map
{
namespace weather
{
namespace rain
{
struct Raindrop {
    glm::vec2 pos;
    float height;

    Raindrop() = default;

    Raindrop(const sf::FloatRect& area) {
        pos.x  = bl::util::Random::get<float>(area.left - 300.f, area.left + area.width + 300.f);
        pos.y  = bl::util::Random::get<float>(area.top - 300.f, area.top + area.height + 300.f);
        height = bl::util::Random::get<float>(120.f, 180.f);
    }
};

struct GpuRaindrop {
    glm::vec2 pos;
    std::uint32_t state; // 0 = air, 1 = splash1, 2 = splash2
    float height;

    GpuRaindrop() = default;

    GpuRaindrop& operator=(const Raindrop& drop) {
        pos    = drop.pos;
        state  = drop.height >= 0.f ? 0 : (drop.height >= SplashTime ? 1 : 2);
        height = drop.height;
        return *this;
    }
};

struct alignas(8) GlobalShaderInfo {
    struct alignas(8) ModeInfo {
        glm::vec2 textureCenter;
        std::uint32_t textureId;
        float radius;
    };

    alignas(8) ModeInfo info[3];
    float rotation;
};
} // namespace rain
} // namespace weather
} // namespace map
} // namespace core

namespace bl
{
namespace pcl
{
using Raindrop         = core::map::weather::rain::Raindrop;
using GpuRaindrop      = core::map::weather::rain::GpuRaindrop;
using GlobalShaderInfo = core::map::weather::rain::GlobalShaderInfo;

template<>
struct RenderConfigMap<Raindrop> {
    static constexpr std::uint32_t PipelineId  = core::Properties::RaindropPipelineId;
    static constexpr bool ContainsTransparency = true;

    static constexpr bool CreateRenderPipeline = true;

    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        bl::pcl::RenderConfigDefaults<Raindrop>::RenderPassIds;

    using GlobalShaderPayload = GlobalShaderInfo;
    using DescriptorSets =
        bl::pcl::RenderConfigDescriptorList<bl::rc::ds::TexturePoolFactory,
                                            bl::rc::ds::Scene2DFactory,
                                            bl::pcl::DescriptorSetFactory<Raindrop, GpuRaindrop>>;

    static constexpr bool EnableDepthTesting      = true;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    static constexpr const char* VertexShader     = "Resources/Shaders/Particles/raindrop.vert.spv";
    static constexpr const char* FragmentShader   = "Resources/Shaders/Particles/raindrop.frag.spv";
};
} // namespace pcl
} // namespace bl

namespace core
{
namespace map
{
namespace weather
{
namespace rain
{
class GravityAffector : public bl::pcl::Affector<Raindrop> {
public:
    GravityAffector(const glm::vec2& vel, float fallVel)
    : vel(vel)
    , fallVel(fallVel) {}

    virtual ~GravityAffector() = default;

    virtual void update(Proxy& proxy, float dt, float) override {
        for (Raindrop& drop : proxy.particles()) {
            drop.pos += vel * dt;
            if (drop.height > 0.f) {
                drop.height -= fallVel * dt;
                if (drop.height < 0.f) { drop.height = 0.f; }
            }
            else { drop.height -= dt; }
        }
    }

private:
    const glm::vec2 vel;
    const float fallVel;
};

class TimeSink : public bl::pcl::Sink<Raindrop> {
public:
    virtual ~TimeSink() = default;

    virtual void update(Proxy& proxy, std::span<Raindrop> particles, float, float) override {
        for (Raindrop& drop : particles) {
            if (drop.height <= DeadTime) { proxy.destroy(drop); }
        }
    }
};

class TimeEmitter : public bl::pcl::Emitter<Raindrop> {
public:
    TimeEmitter(bl::rc::Observer& observer, std::size_t target)
    : observer(observer)
    , target(target)
    , residual(0.f) {}

    virtual ~TimeEmitter() = default;

    void setTarget(std::size_t t) { target = t; }

    virtual void update(Proxy& proxy, float dt, float) override {
        const std::size_t current = proxy.getManager().getParticleCount();
        if (current < target) {
            residual += SpawnRate * dt;
            if (residual >= 1.f) {
                const std::size_t maxSpawn = target - current;
                const std::size_t toSpawn =
                    std::min(static_cast<std::size_t>(std::floor(residual)), maxSpawn);
                residual -= static_cast<float>(toSpawn);

                // TODO - density based target count?
                const sf::FloatRect area =
                    observer.getCurrentCamera<bl::cam::Camera2D>()->getVisibleArea();
                for (std::size_t i = 0; i < toSpawn; ++i) { proxy.emit(area); }
            }
        }
    }

private:
    bl::rc::Observer& observer;
    std::size_t target;
    float residual;
};

} // namespace rain

Rain::Rain(bool hard, bool canThunder)
: engine(nullptr)
, particles(nullptr)
, _type(hard ? (canThunder ? Weather::HardRainThunder : Weather::HardRain) :
               (canThunder ? Weather::LightRainThunder : Weather::LightRain))
, targetParticleCount(hard ? Properties::HardRainParticleCount() :
                             Properties::LightRainParticleCount())
, velocity(hard ? glm::vec2{-90.f, 30.f} : glm::vec2{0.f, 0.f})
, fallSpeed(hard ? 740.f : 500.f)
, rotation(bl::math::degreesToRadians(hard ? 45.f : 15.f))
, stopFactor(-1.f)
, thunder(canThunder, hard) {
    rainSoundHandle = bl::audio::AudioSystem::getOrLoadSound(
        hard ? Properties::HardRainSoundFile() : Properties::LightRainSoundFile());
}

Rain::~Rain() {
    stop();
    if (engine) { engine->particleSystem().removeUniqueSystem<rain::Raindrop>(); }
}

Weather::Type Rain::type() const { return _type; }

void Rain::start(bl::engine::Engine& e, Map& map) {
    engine = &e;

    particles = &e.particleSystem().getUniqueSystem<rain::Raindrop>();

    const auto sampler = e.renderer().vulkanState().samplerCache.noFilterBorderClamped();
    auto& tpool        = e.renderer().texturePool();
    dropTxtr           = tpool.getOrLoadTexture(Properties::RainDropFile(), sampler);
    splash1Txtr        = tpool.getOrLoadTexture(Properties::RainSplash1File(), sampler);
    splash2Txtr        = tpool.getOrLoadTexture(Properties::RainSplash2File(), sampler);

    const bl::rc::res::TextureRef* textures[] = {&dropTxtr, &splash1Txtr, &splash2Txtr};
    for (unsigned int i = 0; i < 3; ++i) {
        auto& tex  = *textures[i];
        auto& info = particles->getRenderer().getGlobals().info[i];

        sf::FloatRect bounds(0.f, 0.f, tex->size().x, tex->size().y);
        sf::Transform transform;
        transform.rotate(bl::math::radiansToDegrees(rotation));
        bounds = transform.transformRect(bounds);
        const glm::vec2 rotSize(bounds.width, bounds.height);

        info.textureId     = tex.id();
        info.textureCenter = tex->normalizeAndConvertCoord(tex->size() * 0.5f);
        info.radius        = glm::length(glm::vec2(bounds.width, bounds.height)) * 0.5f;
    }
    particles->getRenderer().getGlobals().rotation = rotation;

    emitter =
        particles->addEmitter<rain::TimeEmitter>(e.renderer().getObserver(), targetParticleCount);
    particles->addAffector<rain::GravityAffector>(velocity, fallSpeed);
    particles->addSink<rain::TimeSink>();
    particles->addToScene(e.renderer().getObserver().getCurrentScene());
    particles->getRenderer().getComponent()->vertexBuffer.vertices()[0].pos.z = map.getMinDepth();

    bl::audio::AudioSystem::playSound(rainSoundHandle, 1.5f, true);
}

void Rain::stop() {
    stopFactor = 0.f;
    bl::audio::AudioSystem::stopSound(rainSoundHandle, StopTime + 0.5f);
    thunder.stop();
}

bool Rain::stopped() const { return particles && particles->getParticleCount() == 0; }

void Rain::update(float dt) {
    thunder.update(dt);

    if (stopFactor >= 0.f) {
        stopFactor = std::min(stopFactor + StopSpeed * dt, 1.f);
        emitter->setTarget(
            targetParticleCount -
            static_cast<std::size_t>(static_cast<float>(targetParticleCount) * stopFactor));
        if (stopFactor >= 1.f) {
            stopFactor = -1.f;
            particles->removeAllEmitters();
        }
    }
}

} // namespace weather
} // namespace map
} // namespace core
