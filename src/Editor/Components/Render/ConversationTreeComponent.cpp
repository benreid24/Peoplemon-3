#include <Editor/Components/Render/ConversationTreeComponent.hpp>

#include <Core/Files/Conversation.hpp>
#include <Editor/Components/ConversationTree.hpp>

namespace editor
{
namespace component
{
namespace rdr
{
ConversationTreeComponent::ConversationTreeComponent()
: Component(HighlightState::IgnoresMouse)
, enginePtr(nullptr)
, currentOverlay(nullptr) {}

void ConversationTreeComponent::setVisible(bool visible) { background.setHidden(!visible); }

void ConversationTreeComponent::onElementUpdated() {
    // TODO - sync shapes + text
    // TODO - parent text to shapeBatch for move
    // TODO - how to zoom?
}

void ConversationTreeComponent::onRenderSettingChange() {}

bl::ecs::Entity ConversationTreeComponent::getEntity() const { return background.entity(); }

void ConversationTreeComponent::doCreate(bl::engine::Engine& engine,
                                         bl::gui::rdr::Renderer& renderer) {
    enginePtr = &engine;

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
    shapeBatch.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    for (auto& label : nodeLabels) { label.addToScene(overlay, bl::rc::UpdateSpeed::Static); }
}

void ConversationTreeComponent::doSceneRemove() { background.removeFromScene(); }

void ConversationTreeComponent::handleAcquisition() {
    const auto& acq = getOwnerAs<ConversationTree>().getAcquisition();
    background.setSize({acq.width, acq.height});
    handleMove();
}

void ConversationTreeComponent::handleMove() {
    const auto pos = getOwnerAs<ConversationTree>().getLocalPosition();
    background.getTransform().setPosition(pos.x, pos.y);
}

} // namespace rdr
} // namespace component
} // namespace editor
