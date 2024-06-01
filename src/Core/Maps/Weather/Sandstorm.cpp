#include "Sandstorm.hpp"

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace map
{
namespace weather
{
namespace sandstorm
{
struct Sand {
    glm::vec2 pos;

    Sand() = default;

    Sand(const glm::vec2& pos)
    : pos(pos) {}
};

struct Swirl {
    glm::vec2 pos;
    float angle;
    float angularVelocity;
    float scale;

    Swirl() = default;

    Swirl(float x, float y)
    : pos(x, y) {
        angle           = bl::util::Random::get<float>(0.f, 2.f * bl::math::Pi);
        angularVelocity = bl::math::degreesToRadians(bl::util::Random::get<float>(560.f, 820.f));
        if (bl::util::Random::chance(1, 2)) { angularVelocity *= -1.f; }
        scale = bl::util::Random::get<float>(0.75f, 1.2f);
    }
};

struct GpuSwirl {
    glm::vec2 pos;
    float angle;
    float scale;

    GpuSwirl() = default;

    GpuSwirl& operator=(const Swirl& swirl) {
        pos   = swirl.pos;
        angle = swirl.angle;
        scale = swirl.scale;
        return *this;
    }
};

struct GlobalShaderInfo {
    std::uint32_t textureId;
    glm::vec2 textureCenter;
    float alpha;
    float radius;
};

} // namespace sandstorm
} // namespace weather
} // namespace map
} // namespace core

namespace bl
{
namespace pcl
{
using Sand             = core::map::weather::sandstorm::Sand;
using Swirl            = core::map::weather::sandstorm::Swirl;
using GpuSwirl         = core::map::weather::sandstorm::GpuSwirl;
using GlobalShaderInfo = core::map::weather::sandstorm::GlobalShaderInfo;

template<>
struct RenderConfigMap<Sand> {
    static constexpr std::uint32_t PipelineId  = core::Properties::SandstormSandPipelineId;
    static constexpr bool ContainsTransparency = true;

    static constexpr bool CreateRenderPipeline = true;

    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        bl::pcl::RenderConfigDefaults<Sand>::RenderPassIds;

    using GlobalShaderPayload = GlobalShaderInfo;
    using DescriptorSets =
        bl::pcl::RenderConfigDescriptorList<bl::rc::ds::TexturePoolFactory,
                                            bl::rc::ds::Scene2DFactory,
                                            bl::pcl::DescriptorSetFactory<Sand, Sand>>;

    static constexpr bool EnableDepthTesting      = true;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    static constexpr const char* VertexShader     = "Resources/Shaders/Particles/sand.vert.spv";
    static constexpr const char* FragmentShader =
        bl::rc::Config::ShaderIds::Fragment2DRotatedParticle;
};

template<>
struct RenderConfigMap<Swirl> {
    static constexpr std::uint32_t PipelineId  = core::Properties::SandstormSwirlPipelineId;
    static constexpr bool ContainsTransparency = true;

    static constexpr bool CreateRenderPipeline = true;

    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        bl::pcl::RenderConfigDefaults<Swirl>::RenderPassIds;

    using GlobalShaderPayload = GlobalShaderInfo;
    using DescriptorSets =
        bl::pcl::RenderConfigDescriptorList<bl::rc::ds::TexturePoolFactory,
                                            bl::rc::ds::Scene2DFactory,
                                            bl::pcl::DescriptorSetFactory<Swirl, GpuSwirl>>;

    static constexpr bool EnableDepthTesting      = true;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    static constexpr const char* VertexShader = "Resources/Shaders/Particles/sandSwirl.vert.spv";
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
namespace
{
constexpr glm::vec2 SandVelocity(-1100.f * 0.8f, 345.f * 0.8f);
constexpr glm::vec2 SwirlVelocity(-680.f, 325.f);
constexpr float MaxAlpha          = 230.f / 256.f;
constexpr float SwirlRatio        = 0.5f;
constexpr float AlphaRate         = 85.f / 256.f;
constexpr unsigned int SwirlCount = 40;

float computeAlpha(float current, float target, float dt) {
    const float rate = target > current ? AlphaRate : -AlphaRate;
    const float na   = current + rate * dt;
    if (na < 0.f) return 0.f;
    if (na > 1.f) return 1.f;
    if (na > target && target > current) return target;
    return na;
}
} // namespace

namespace sandstorm
{
class SandSink : public bl::pcl::Sink<Sand> {
public:
    SandSink()
    : clear(false) {}

    virtual ~SandSink() = default;

    virtual void update(Proxy& proxy, std::span<Sand> particles, float, float) override {
        if (clear) {
            clear = false;
            for (Sand& p : particles) { proxy.destroy(p); }
        }
    }

    void clearAll() { clear = true; }

private:
    bool clear;
};

class SandUpdater : public bl::pcl::MetaUpdater<Sand> {
public:
    SandUpdater(bl::rc::Observer& observer, SandSink& sink)
    : observer(observer)
    , sink(sink)
    , prevArea(1.f, 1.f, 1.f, 1.f) {}

    virtual ~SandUpdater() = default;

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
    bl::rc::Observer& observer;
    SandSink& sink;
    sf::FloatRect prevArea;
};

float wrapWidth  = 800.f;
float wrapHeight = 600.f;

class SandAffector : public bl::pcl::Affector<Sand> {
public:
    SandAffector(bl::rc::Observer& observer)
    : observer(observer) {}

    virtual ~SandAffector() = default;

    virtual void update(Proxy& proxy, float dt, float) override {
        bl::cam::Camera2D* cam = observer.getCurrentCamera<bl::cam::Camera2D>();
        if (!cam) { return; }
        const sf::FloatRect area = cam->getVisibleArea();

        const float leftBound   = area.left + area.width * 0.5f - wrapWidth * 0.5f;
        const float rightBound  = leftBound + wrapWidth;
        const float topBound    = area.top + area.height * 0.5f - wrapHeight * 0.5f;
        const float bottomBound = topBound + wrapHeight;

        for (Sand& s : proxy.particles()) {
            s.pos += SandVelocity * dt;
            if (s.pos.x < leftBound) { s.pos.x += wrapWidth; }
            else if (s.pos.x > rightBound) { s.pos.x -= wrapWidth; }
            if (s.pos.y < topBound) { s.pos.y += wrapHeight; }
            else if (s.pos.y > bottomBound) { s.pos.y -= wrapHeight; }
        }
    }

private:
    bl::rc::Observer& observer;
};

class SandEmitter : public bl::pcl::Emitter<Sand> {
public:
    SandEmitter(bl::rc::Observer& observer, glm::vec2 txtrSize)
    : observer(observer)
    , txtrHalfSize(txtrSize / 1.6f) {}

    virtual ~SandEmitter() = default;

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
                    const glm::vec2 pos(x, y);
                    proxy.emit(corner + txtrHalfSize * pos);
                }
            }
        }
    }

private:
    bl::rc::Observer& observer;
    const glm::vec2 txtrHalfSize;
};

class SwirlAffector : public bl::pcl::Affector<Swirl> {
public:
    SwirlAffector(bl::rc::Observer& observer)
    : observer(observer) {}

    virtual ~SwirlAffector() = default;

    virtual void update(Proxy& proxy, float dt, float) override {
        bl::cam::Camera2D* cam = observer.getCurrentCamera<bl::cam::Camera2D>();
        if (!cam) { return; }
        const sf::FloatRect area = cam->getVisibleArea();

        const float leftBound   = area.left - area.width * 0.5f;
        const float rightBound  = leftBound + area.width * 2.f;
        const float topBound    = area.top - area.height * 0.5f;
        const float bottomBound = topBound + area.height * 2.f;

        for (Swirl& s : proxy.particles()) {
            s.pos += SwirlVelocity * dt;
            s.angle += s.angularVelocity * dt;

            if (s.pos.x < leftBound || s.pos.y > bottomBound) {
                s = Swirl(bl::util::Random::get<float>(rightBound - 100.f, rightBound),
                          bl::util::Random::get<float>(topBound, bottomBound - 100.f));
            }
        }
    }

private:
    bl::rc::Observer& observer;
};

class SwirlEmitter : public bl::pcl::Emitter<Swirl> {
public:
    SwirlEmitter(bl::rc::Observer& observer, glm::vec2 txtrSize)
    : observer(observer)
    , txtrHalfSize(txtrSize / 1.6f) {}

    virtual ~SwirlEmitter() = default;

    virtual void update(Proxy& proxy, float, float) override {
        if (proxy.getManager().getParticleCount() == 0) {
            bl::cam::Camera2D* cam = observer.getCurrentCamera<bl::cam::Camera2D>();
            if (!cam) { return; }
            const sf::FloatRect area = cam->getVisibleArea();
            const glm::vec2 corner(area.left - area.width * 0.5f, area.top - area.height * 0.5f);
            const glm::vec2 otherCorner(corner + glm::vec2(area.width * 2.f, area.height * 2.f));

            for (unsigned int i = 0; i < SwirlCount; ++i) {
                proxy.emit(bl::util::Random::get<float>(corner.x, otherCorner.x),
                           bl::util::Random::get<float>(corner.y, otherCorner.y));
            }
        }
    }

private:
    bl::rc::Observer& observer;
    const glm::vec2 txtrHalfSize;
};
} // namespace sandstorm

Sandstorm::Sandstorm()
: targetAlpha(0.f)
, alpha(0.f) {}

Sandstorm::~Sandstorm() {
    if (engine) {
        engine->particleSystem().removeUniqueSystem<sandstorm::Sand>();
        engine->particleSystem().removeUniqueSystem<sandstorm::Swirl>();
    }
}

Weather::Type Sandstorm::type() const { return Weather::SandStorm; }

void Sandstorm::stop() { targetAlpha = 0; }

bool Sandstorm::stopped() const { return sand && sand->getRenderer().getGlobals().alpha <= 0.f; }

void Sandstorm::start(bl::engine::Engine& e, Map& map) {
    engine      = &e;
    targetAlpha = MaxAlpha;
    alpha       = 0.f;

    const auto sampler = e.renderer().vulkanState().samplerCache.noFilterBorderClamped();
    sandTxtr  = e.renderer().texturePool().getOrLoadTexture(Properties::SandPatchFile(), sampler);
    swirlTxtr = e.renderer().texturePool().getOrLoadTexture(Properties::SandSwirlFile(), sampler);

    // sand
    sand                      = &e.particleSystem().getUniqueSystem<sandstorm::Sand>();
    sandstorm::SandSink* sink = sand->addSink<sandstorm::SandSink>();
    sand->addUpdater<sandstorm::SandUpdater>(std::ref(e.renderer().getObserver()), std::ref(*sink));
    sand->addAffector<sandstorm::SandAffector>(e.renderer().getObserver());
    sand->addEmitter<sandstorm::SandEmitter>(e.renderer().getObserver(), sandTxtr->size());

    sand->getRenderer().getGlobals().textureCenter =
        sandTxtr->normalizeAndConvertCoord(sandTxtr->size() * 0.5f);
    sand->getRenderer().getGlobals().textureId = sandTxtr.id();
    sand->getRenderer().getGlobals().radius    = glm::length(sandTxtr->size()) * 0.5f;

    sand->addToScene(e.renderer().getObserver().getCurrentScene());
    sand->getRenderer().getComponent()->vertexBuffer.vertices()[0].pos.z = map.getMinDepth() + 0.3f;

    // swirls
    swirls = &e.particleSystem().getUniqueSystem<sandstorm::Swirl>();
    swirls->addAffector<sandstorm::SwirlAffector>(e.renderer().getObserver());
    swirls->addEmitter<sandstorm::SwirlEmitter>(e.renderer().getObserver(), sandTxtr->size());

    swirls->getRenderer().getGlobals().textureCenter =
        swirlTxtr->normalizeAndConvertCoord(swirlTxtr->size() * 0.5f);
    swirls->getRenderer().getGlobals().textureId = swirlTxtr.id();
    swirls->getRenderer().getGlobals().radius    = glm::length(swirlTxtr->size()) * 0.5f;

    swirls->addToScene(e.renderer().getObserver().getCurrentScene());
    swirls->getRenderer().getComponent()->vertexBuffer.vertices()[0].pos.z = map.getMinDepth();

    setAlpha();
}

void Sandstorm::update(float dt) {
    if (alpha != targetAlpha) {
        alpha = computeAlpha(alpha, targetAlpha, dt);
        setAlpha();
    }
}

void Sandstorm::setAlpha() {
    if (sand) {
        sand->getRenderer().getGlobals().alpha   = alpha;
        swirls->getRenderer().getGlobals().alpha = alpha * SwirlRatio;
    }
}

} // namespace weather
} // namespace map
} // namespace core
