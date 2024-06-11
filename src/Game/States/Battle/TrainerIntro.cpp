#include "TrainerIntro.hpp"

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
constexpr float IntroLength = 1.6f;

struct FragmentUniform {
    glm::vec2 windowSize;
    float progress; // [0, 1]
};

using Bindings = bl::rc::ds::Bindings<bl::rc::ds::GlobalUniformBuffer<FragmentUniform>>;
using Factory =
    bl::rc::ds::GenericDescriptorSetFactory<Bindings, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT>;
using Instance = bl::rc::ds::GenericDescriptorSetInstance<Bindings>;

} // namespace

TrainerSequence::TrainerSequence()
: time(0.f) {}

void TrainerSequence::start(bl::engine::Engine& engine) {
    static bool pipelineCreated = false;
    if (!pipelineCreated) {
        pipelineCreated = true;

        bl::rc::vk::PipelineParameters params;
        params.withSimpleDepthStencil(false)
            .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .addDescriptorSet<bl::rc::ds::Scene2DFactory>()
            .addDescriptorSet<bl::rc::ds::Object2DFactory>()
            .addDescriptorSet<Factory>()
            .withShaders(bl::rc::Config::ShaderIds::Vertex2D,
                         "Resources/Shaders/Battle/trainerIntro.frag.spv");
        engine.renderer().pipelineCache().createPipline(core::Properties::TrainerIntroPipelineId,
                                                        params.build());
    }

    // TODO - play sound and start music

    auto overlay                    = engine.renderer().getObserver().getOrCreateSceneOverlay();
    e                               = &engine;
    time                            = 0.f;
    const sf::Vector2f screenCenter = core::Properties::WindowSize() * 0.5f;

    background.create(engine, {core::Properties::WindowSize().x, core::Properties::WindowSize().y});
    background.setFillColor(sf::Color::Black);
    background.addToSceneWithCustomPipeline(
        overlay, bl::rc::UpdateSpeed::Static, core::Properties::TrainerIntroPipelineId);

    ballTxtr = engine.renderer().texturePool().getOrLoadTexture(
        bl::util::FileUtil::joinPath(core::Properties::ImagePath(), "Battle/battleBall.png"));
    ball.create(engine, ballTxtr);
    ball.getTransform().setOrigin(ballTxtr->size() * 0.5f);
    ball.getTransform().setPosition({screenCenter.x, screenCenter.y});
    ball.getTransform().setScale(0.f, 0.f);
    ball.setParent(background);
    ball.addToScene(overlay, bl::rc::UpdateSpeed::Dynamic);

    Instance& set = overlay->getDescriptorSet<Instance>();
    uniform       = &set.getBindingPayload<FragmentUniform>();
}

void TrainerSequence::update(float dt) {
    time += dt;

    FragmentUniform* u    = static_cast<FragmentUniform*>(uniform);
    const sf::Vector2u ws = e->window().getSfWindow().getSize();
    u->progress           = time / IntroLength;
    u->windowSize.x       = static_cast<float>(ws.x);
    u->windowSize.y       = static_cast<float>(ws.y);

    ball.getTransform().setScale(u->progress);
}

bool TrainerSequence::finished() const { return time >= IntroLength; }

} // namespace intros
} // namespace state
} // namespace game
