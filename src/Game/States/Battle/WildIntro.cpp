#include "WildIntro.hpp"

#include <BLIB/Render.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
namespace intros
{
namespace
{
constexpr float IntroLength                  = 1.6f;
bool pipelineCreated                         = false;
constexpr std::array<glm::vec4, 6> BarColors = {glm::vec4(235.f, 64.f, 52.f, 255.f) / 255.f,
                                                glm::vec4(38.f, 168.f, 34.f, 255.f) / 255.f,
                                                glm::vec4(240.f, 185.f, 77.f, 255.f) / 255.f,
                                                glm::vec4(39.f, 86.f, 217.f, 255.f) / 255.f,
                                                glm::vec4(219.f, 219.f, 9.f, 255.f) / 255.f,
                                                glm::vec4(219.f, 9.f, 202.f, 255.f) / 255.f};

struct FragmentUniform {
    glm::vec2 windowSize;
    float progress;
    std::uint32_t base;
    std::array<glm::vec4, 6> colors;
};

using Bindings = bl::rc::ds::Bindings<bl::rc::ds::GlobalUniformBuffer<FragmentUniform>>;
using Factory  = bl::rc::ds::GenericDescriptorSetFactory<Bindings, VK_SHADER_STAGE_FRAGMENT_BIT>;
using Instance = bl::rc::ds::GenericDescriptorSetInstance<Bindings>;

} // namespace

WildSequence::WildSequence()
: time(0.f)
, shuffleTime(0.f) {}

void WildSequence::start(bl::engine::Engine& engine) {
    // TODO - play sound and start music
    time        = 0.f;
    shuffleTime = 0.f;

    if (!pipelineCreated) {
        pipelineCreated = true;

        bl::rc::vk::PipelineParameters params;
        params.withSimpleDepthStencil(false)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .addDescriptorSet<bl::rc::ds::Scene2DFactory>()
            .addDescriptorSet<bl::rc::ds::Object2DFactory>()
            .addDescriptorSet<Factory>()
            .withShaders(bl::rc::Config::ShaderIds::Vertex2D,
                         "Resources/Shaders/Battle/wildIntro.frag.spv");
        engine.renderer().pipelineCache().createPipline(core::Properties::WildIntroPipelineId,
                                                        params.build());
    }

    auto overlay                    = engine.renderer().getObserver().getOrCreateSceneOverlay();
    const sf::Vector2f screenCenter = core::Properties::WindowSize() * 0.5f;

    background.create(engine, {core::Properties::WindowSize().x, core::Properties::WindowSize().y});
    background.addToSceneWithCustomPipeline(
        overlay, bl::rc::UpdateSpeed::Static, core::Properties::WildIntroPipelineId);

    Instance& set        = overlay->getDescriptorSet<Instance>();
    FragmentUniform& uni = set.getBindingPayload<FragmentUniform>();
    uni.base             = 0;
    uni.colors           = BarColors;
    uni.progress         = 0.f;

    e       = &engine;
    uniform = &uni;
}

void WildSequence::update(float dt) {
    constexpr float ShuffleTime = 0.085f;

    time += dt;
    shuffleTime += dt;

    FragmentUniform* u    = static_cast<FragmentUniform*>(uniform);
    const sf::Vector2u ws = e->window().getSfWindow().getSize();
    u->progress           = time / IntroLength;
    u->windowSize.x       = static_cast<float>(ws.x);
    u->windowSize.y       = static_cast<float>(ws.y);

    if (shuffleTime >= ShuffleTime) {
        u->base     = (u->base + 1) % BarColors.size();
        shuffleTime = 0.f;
    }
}

// void WildSequence::render(sf::RenderTarget& target, float lag) {
//     // compute progress and positions
//     const float t              = time + lag;
//     const float progress       = t / IntroLength;
//     const sf::View& view       = target.getView();
//     const sf::Vector2f scorner = view.getCenter() - view.getSize() * 0.5f;
//     const sf::Vector2f ecorner = scorner + view.getSize();
//
//     // render bars
//     const float bw = view.getSize().x * 0.5f * progress;
//     unsigned int i = si;
//     for (float y = scorner.y + barCircle.getRadius(); y <= ecorner.y - barCircle.getRadius();
//          y += barCircle.getRadius() * 2.f) {
//         i = i < barColors.size() - 1 ? i + 1 : 0;
//         barCircle.setFillColor(barColors[i]);
//         for (float x = 0.f; x <= bw; x += 1.f) {
//             barCircle.setPosition(scorner.x + x, y);
//             target.draw(barCircle);
//             barCircle.setPosition(ecorner.x - x, y);
//             target.draw(barCircle);
//         }
//     }
// }

bool WildSequence::finished() const { return time >= 2.5f; }

} // namespace intros
} // namespace state
} // namespace game
