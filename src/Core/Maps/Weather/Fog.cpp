#include "Fog.hpp"

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
constexpr float AlphaRate = 0.1f;
const glm::vec2 FogVelocity(64.f, 25.f);

float computeAlpha(float current, float target, float step) {
    const float rate = target > current ? AlphaRate : -AlphaRate;
    const float na   = current + rate * step;
    if (na < 0.f) return 0.f;
    if (na > 1.f) return 1.f;
    if (current < target && na > target) return target;
    return na;
}

} // namespace

namespace fog
{
struct Particle {
    glm::vec2 pos;
    float rotation;
    float angularVelocity;
    float scale;

    Particle() = default;

    Particle(const glm::vec2& pos)
    : pos(pos)
    , rotation(bl::util::Random::get<float>(0.f, 2.f * bl::math::Pi))
    , angularVelocity(bl::util::Random::get<float>(-bl::math::Pi, bl::math::Pi))
    , scale(bl::util::Random::get<float>(0.8f, 1.8f)) {}
};

struct GpuParticle {
    glm::vec2 pos;
    float rotation;
    float scale;

    GpuParticle() = default;

    GpuParticle& operator=(const Particle& p) {
        pos      = p.pos;
        rotation = p.rotation;
        scale    = p.scale;
        return *this;
    }
};

struct GlobalShaderInfo {
    std::uint32_t textureId;
    glm::vec2 textureCenter;
    float alpha;
    float radius;
};

} // namespace fog
} // namespace weather
} // namespace map
} // namespace core

namespace bl
{
namespace pcl
{
using Particle         = core::map::weather::fog::Particle;
using GpuParticle      = core::map::weather::fog::GpuParticle;
using GlobalShaderInfo = core::map::weather::fog::GlobalShaderInfo;

template<>
struct RenderConfigMap<Particle> {
    static constexpr std::uint32_t PipelineId  = core::Properties::FogPipelineId;
    static constexpr bool ContainsTransparency = true;

    static constexpr bool CreateRenderPipeline = true;

    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        bl::pcl::RenderConfigDefaults<Particle>::RenderPassIds;

    using GlobalShaderPayload = GlobalShaderInfo;
    using DescriptorSets =
        bl::pcl::RenderConfigDescriptorList<bl::rc::ds::TexturePoolFactory,
                                            bl::rc::ds::Scene2DFactory,
                                            bl::pcl::DescriptorSetFactory<Particle, GpuParticle>>;

    static constexpr bool EnableDepthTesting      = true;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    static constexpr const char* VertexShader     = "Resources/Shaders/Particles/fog.vert.spv";
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
namespace fog
{
class FogSink : public bl::pcl::Sink<fog::Particle> {
public:
    FogSink()
    : clear(false) {}

    virtual ~FogSink() = default;

    virtual void update(Proxy& proxy, std::span<Particle> particles, float, float) override {
        if (clear) {
            clear = false;
            for (Particle& p : particles) { proxy.destroy(p); }
        }
    }

    void clearAll() { clear = true; }

private:
    bool clear;
};

class FogUpdater : public bl::pcl::MetaUpdater<fog::Particle> {
public:
    FogUpdater(bl::rc::RenderTarget& observer, FogSink& sink)
    : observer(observer)
    , sink(sink)
    , prevArea(1.f, 1.f, 1.f, 1.f) {}

    virtual ~FogUpdater() = default;

    virtual void update(Proxy& proxy, float, float) override {
        bl::cam::Camera2D* cam = observer.getCurrentCamera<bl::cam::Camera2D>();
        if (!cam) { return; }
        const sf::FloatRect area = cam->getVisibleArea();

        if (proxy.getManager().getParticleCount() > 0) {
            const float dx = area.width / prevArea.width;
            const float dy = area.height / prevArea.height;
            if (dx >= 1.25f || dy >= 1.25f) {
                sink.clearAll();
                prevArea = area;
            }
        }
        else { prevArea = area; }
    }

private:
    bl::rc::RenderTarget& observer;
    FogSink& sink;
    sf::FloatRect prevArea;
};

float wrapWidth  = 800.f;
float wrapHeight = 600.f;

class FogAffector : public bl::pcl::Affector<fog::Particle> {
public:
    FogAffector(bl::rc::RenderTarget& observer)
    : observer(observer) {}

    virtual ~FogAffector() = default;

    virtual void update(Proxy& proxy, float dt, float) override {
        bl::cam::Camera2D* cam = observer.getCurrentCamera<bl::cam::Camera2D>();
        if (!cam) { return; }

        const sf::FloatRect area = cam->getVisibleArea();
        const float leftBound    = area.left + area.width * 0.5f - wrapWidth * 0.5f;
        const float rightBound   = leftBound + wrapWidth;
        const float topBound     = area.top + area.height * 0.5f - wrapHeight * 0.5f;
        const float bottomBound  = topBound + wrapHeight;

        for (Particle& p : proxy.particles()) {
            p.pos += FogVelocity * dt;
            p.rotation += p.angularVelocity * dt;
            if (p.pos.x < leftBound) { p.pos.x += wrapWidth; }
            else if (p.pos.x > rightBound) { p.pos.x -= wrapWidth; }
            if (p.pos.y < topBound) { p.pos.y += wrapHeight; }
            else if (p.pos.y > bottomBound) { p.pos.y -= wrapHeight; }
        }
    }

private:
    bl::rc::RenderTarget& observer;
};

class FogEmitter : public bl::pcl::Emitter<fog::Particle> {
public:
    FogEmitter(bl::rc::RenderTarget& observer, glm::vec2 txtrSize)
    : observer(observer)
    , txtrHalfSize(txtrSize * 0.4f) {}

    virtual ~FogEmitter() = default;

    virtual void update(Proxy& proxy, float, float) override {
        if (proxy.getManager().getParticleCount() == 0) {
            bl::cam::Camera2D* cam = observer.getCurrentCamera<bl::cam::Camera2D>();
            if (!cam) { return; }
            const sf::FloatRect area = cam->getVisibleArea();

            const unsigned int width  = std::ceil(area.width * 2.f / txtrHalfSize.x);
            const unsigned int height = std::ceil(area.height * 2.f / txtrHalfSize.y);

            wrapWidth  = static_cast<float>(width) * txtrHalfSize.x;
            wrapHeight = static_cast<float>(height) * txtrHalfSize.y;
            const glm::vec2 corner(area.left + area.width * 0.5f - wrapWidth * 0.5f,
                                   area.top + area.height * 0.5f - wrapHeight * 0.5f);

            for (unsigned int x = 0; x < width; ++x) {
                for (unsigned int y = 0; y < height; ++y) {
                    const glm::vec2 pos{static_cast<float>(x), static_cast<float>(y)};
                    proxy.emit(pos * txtrHalfSize + corner);
                }
            }
        }
    }

private:
    bl::rc::RenderTarget& observer;
    const glm::vec2 txtrHalfSize;
};

} // namespace fog

Fog::Fog(bool thick)
: engine(nullptr)
, maxOpacity(static_cast<float>(thick ? Properties::ThickFogAlpha() : Properties::ThinFogAlpha()) /
             255.f)
, targetOpacity(0.f)
, particles(nullptr) {}

Fog::~Fog() {
    if (engine) { engine->particleSystem().removeUniqueSystem<fog::Particle>(); }
}

Weather::Type Fog::type() const {
    return maxOpacity == Properties::ThickFogAlpha() ? Weather::ThickFog : Weather::ThinFog;
}

void Fog::start(bl::engine::Engine& e, bl::rc::RenderTarget& renderTarget, Map& map) {
    engine        = &e;
    targetOpacity = maxOpacity;

    const auto sampler = e.renderer().vulkanState().samplerCache.noFilterBorderClamped();
    fogTxtr = e.renderer().texturePool().getOrLoadTexture(Properties::FogFile(), sampler);

    particles          = &e.particleSystem().getUniqueSystem<fog::Particle>();
    fog::FogSink* sink = particles->addSink<fog::FogSink>();
    particles->addUpdater<fog::FogUpdater>(std::ref(renderTarget), std::ref(*sink));
    particles->addAffector<fog::FogAffector>(renderTarget);
    particles->addEmitter<fog::FogEmitter>(renderTarget, fogTxtr->size());

    particles->getRenderer().getGlobals().alpha = 0.f;
    particles->getRenderer().getGlobals().textureCenter =
        fogTxtr->normalizeAndConvertCoord(fogTxtr->size() * 0.5f);
    particles->getRenderer().getGlobals().textureId = fogTxtr.id();
    particles->getRenderer().getGlobals().radius    = glm::length(fogTxtr->size()) * 0.5f;

    particles->addToScene(renderTarget.getCurrentScene());
    particles->getRenderer().getComponent()->vertexBuffer.vertices()[0].pos.z = map.getMinDepth();
}

void Fog::stop() { targetOpacity = 0.f; }

bool Fog::stopped() const {
    return particles && particles->getRenderer().getGlobals().alpha <= 0.01f;
}

void Fog::update(float dt) {
    if (particles) {
        if (particles->getRenderer().getGlobals().alpha != targetOpacity) {
            particles->getRenderer().getGlobals().alpha =
                computeAlpha(particles->getRenderer().getGlobals().alpha, targetOpacity, dt);
        }
    }
}

} // namespace weather
} // namespace map
} // namespace core
