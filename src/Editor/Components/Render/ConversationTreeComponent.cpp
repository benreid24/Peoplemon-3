#include <Editor/Components/Render/ConversationTreeComponent.hpp>

#include <BLIB/Engine.hpp>
#include <Core/Files/Conversation.hpp>
#include <Core/Properties.hpp>
#include <Editor/Components/ConversationTree.hpp>

namespace editor
{
namespace component
{
namespace rdr
{
namespace
{
bool pipelinesCreated = false;

using CustomBindings =
    bl::rc::ds::Bindings<bl::rc::ds::GlobalUniformBuffer<ConversationTreeComponent::TreeCamera>>;
using CustomSet = bl::rc::ds::GenericDescriptorSetInstance<CustomBindings>;
using CustomSetFactory =
    bl::rc::ds::GenericDescriptorSetFactory<CustomBindings, VK_SHADER_STAGE_VERTEX_BIT>;

void ensurePipelinesCreated(bl::engine::Engine& engine) {
    if (!pipelinesCreated) {
        pipelinesCreated = true;

        auto& cache = engine.renderer().pipelineCache();
        cache.createPipline(core::Properties::EditorConversationTreeShapePipelineId,
                            bl::rc::vk::PipelineParameters()
                                .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                .withSimpleDepthStencil(true)
                                .addDescriptorSet<bl::rc::ds::TexturePoolFactory>()
                                .addDescriptorSet<bl::rc::ds::Scene2DFactory>()
                                .addDescriptorSet<bl::rc::ds::Object2DFactory>()
                                .addDescriptorSet<CustomSetFactory>()
                                .withShaders("Resources/Shaders/Editor/conversationTree.vert.spv",
                                             bl::rc::Config::ShaderIds::Fragment2DSkinnedLit)
                                .build());
        cache.createPipline(core::Properties::EditorConversationTreeTextPipelineId,
                            bl::rc::vk::PipelineParameters()
                                .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                .withSimpleDepthStencil(true)
                                .addDescriptorSet<bl::rc::ds::TexturePoolFactory>()
                                .addDescriptorSet<bl::rc::ds::Scene2DFactory>()
                                .addDescriptorSet<bl::rc::ds::Object2DFactory>()
                                .addDescriptorSet<CustomSetFactory>()
                                .withShaders("Resources/Shaders/Editor/conversationTree.vert.spv",
                                             bl::rc::Config::ShaderIds::TextFragment)
                                .build());
    }
}

const sf::Color NodeColor(80, 90, 230);
const sf::Color TerminatorColor(230, 70, 120);
const sf::Color SelectedNodeColor(190, 190, 70);
const sf::Color DownArrowColor(30, 220, 65);
const sf::Color UpArrowColor(230, 90, 40);

constexpr float ArrowWidth = 5.f;
constexpr float ArrowDepth = 18.f;

} // namespace
ConversationTreeComponent::ConversationTreeComponent()
: Component(HighlightState::IgnoresMouse)
, enginePtr(nullptr)
, currentOverlay(nullptr)
, lastTreeVersion(std::numeric_limits<unsigned int>::max())
, uniform(nullptr) {}

void ConversationTreeComponent::setVisible(bool visible) { background.setHidden(!visible); }

void ConversationTreeComponent::onElementUpdated() {
    auto& tree = getOwnerAs<ConversationTree>();

    // Update nodes, arrows, and text if changed
    if (lastTreeVersion != tree.getTreeVersion()) {
        lastTreeVersion = tree.getTreeVersion();

        // circles
        nodeCircles.resize(tree.getNodes().size());
        nodeLabels.resize(tree.getNodes().size());
        unsigned int i = 0;
        for (auto& src : tree.getNodes()) {
            auto& circle = nodeCircles[i];
            if (!circle.isCreated()) {
                circle.create(*enginePtr, shapeBatch, ConversationTree::NodeRadius);
                circle.getLocalTransform().setOrigin(ConversationTree::NodeRadius,
                                                     ConversationTree::NodeRadius);
                circle.setFillColor(NodeColor);
            }
            circle.getLocalTransform().setPosition(src.center);

            auto& lbl = nodeLabels[i];
            if (!lbl) {
                lbl = std::make_unique<bl::gfx::Text>();
                lbl->create(*enginePtr,
                            core::Properties::MenuFont(),
                            src.label,
                            18,
                            sf::Color::Black,
                            sf::Text::Style::Bold);
                lbl->setParent(shapeBatch);
                if (currentOverlay) {
                    lbl->addToSceneWithCustomPipeline(
                        currentOverlay,
                        bl::rc::UpdateSpeed::Static,
                        core::Properties::EditorConversationTreeTextPipelineId);
                }
            }
            else { lbl->getSection().setString(src.label); }
            lbl->getTransform().setOrigin(lbl->getLocalSize() * 0.5f);
            lbl->getTransform().setPosition(src.center);

            ++i;
        }

        // arrows
        constexpr float SrcSize = 25.f;
        nodeArrows.resize(tree.getEdges().size(),
                          bl::gfx::BatchIcon(bl::gfx::BatchIcon::Type::Arrow, {SrcSize, SrcSize}));
        i = 0;
        for (auto& src : tree.getEdges()) {
            auto& arrow = nodeArrows[i];
            ++i;

            if (!arrow.isCreated()) {
                arrow.create(*enginePtr, shapeBatch);
                arrow.getLocalTransform().setOrigin(SrcSize * 0.5f, SrcSize * 0.5f);
            }

            const auto& from = tree.getNodes()[src.from];
            const auto& to   = tree.getNodes()[src.to];

            const glm::vec2 pdiff = to.center - from.center;
            const float gap = glm::length(pdiff) - ConversationTree::NodeRadius * 2.f + ArrowDepth;
            const float angle = std::atan2f(pdiff.y, pdiff.x);
            arrow.getLocalTransform().setPosition(from.center + pdiff * 0.5f);
            arrow.getLocalTransform().setRotation(bl::math::radiansToDegrees(angle));
            // TODO - need to account for rotation?
            arrow.getLocalTransform().setScale(gap / SrcSize, ArrowWidth / SrcSize);
            arrow.setFillColor(src.to > src.from ? DownArrowColor : UpArrowColor);
        }
    }

    // Always update camera and colors
    const auto* srcNode = &tree.getNodes()[0];
    unsigned int i      = 0;
    for (auto& node : nodeCircles) {
        sf::Color col = srcNode->terminator ? TerminatorColor : NodeColor;
        if (tree.shouldHighlightSelectedNode() && i == tree.getSelectedNode()) {
            col = SelectedNodeColor;
        }
        const glm::vec4 gcol = bl::sfcol(col);
        if (gcol != node.getFillColor()) { node.setFillColor(gcol); }

        ++i;
        ++srcNode;
    }

    if (uniform) {
        uniform->center    = tree.getCamCenter();
        uniform->zoom      = tree.getCamZoom();
        uniform->acqSize.x = tree.getAcquisition().width;
        uniform->acqSize.y = tree.getAcquisition().height;
    }
}

void ConversationTreeComponent::onRenderSettingChange() {}

bl::ecs::Entity ConversationTreeComponent::getEntity() const { return background.entity(); }

void ConversationTreeComponent::doCreate(bl::engine::Engine& engine, bl::gui::rdr::Renderer&) {
    enginePtr = &engine;
    ensurePipelinesCreated(engine);

    background.create(engine, {100.f, 100.f});
    background.setFillColor(sf::Color::Cyan);
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(-2.f);
    background.getOverlayScaler().setScissorMode(bl::com::OverlayScaler::ScissorSelfConstrained);

    shapeBatch.create(engine, 2048);
    shapeBatch.setParent(background);
}

void ConversationTreeComponent::doSceneAdd(bl::rc::Overlay* overlay) {
    currentOverlay = overlay;

    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    shapeBatch.addToSceneWithCustomPipeline(
        overlay,
        bl::rc::UpdateSpeed::Static,
        core::Properties::EditorConversationTreeShapePipelineId);
    for (auto& label : nodeLabels) {
        label->addToSceneWithCustomPipeline(overlay,
                                            bl::rc::UpdateSpeed::Static,
                                            core::Properties::EditorConversationTreeTextPipelineId);
    }

    uniform = &overlay->getDescriptorSet<CustomSet>().getBindingPayload<TreeCamera>();
}

void ConversationTreeComponent::doSceneRemove() { background.removeFromScene(); }

void ConversationTreeComponent::handleAcquisition() {
    const auto& acq = getOwnerAs<ConversationTree>().getAcquisition();
    background.setSize({acq.width, acq.height});
    handleMove();
    if (uniform) {
        uniform->acqSize.x = acq.width;
        uniform->acqSize.y = acq.height;
    }
}

void ConversationTreeComponent::handleMove() {
    const auto pos = getOwnerAs<ConversationTree>().getLocalPosition();
    background.getTransform().setPosition(pos.x, pos.y);
}

} // namespace rdr
} // namespace component
} // namespace editor
