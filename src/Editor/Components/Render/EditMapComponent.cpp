#include <Editor/Components/Render/EditMapComponent.hpp>

#include <Core/Properties.hpp>
#include <Editor/Components/EditMap.hpp>

namespace editor
{
namespace component
{
namespace rdr
{
EditMapComponent::EditMapComponent()
: Component(HighlightState::IgnoresMouse)
, enginePtr(nullptr) {}

void EditMapComponent::setVisible(bool visible) { sprite.setHidden(!visible); }

void EditMapComponent::onElementUpdated() {
    EditMap& owner = getOwnerAs<EditMap>();
    if (renderTexture->getCurrentScene() != owner.scene) {
        if (renderTexture->sceneCount() > 0) { renderTexture->popScene(); }
        renderTexture->pushScene(owner.scene);
    }
}

void EditMapComponent::onRenderSettingChange() {
    // noop
}

bl::ecs::Entity EditMapComponent::getEntity() const { return sprite.entity(); }

void EditMapComponent::doCreate(bl::engine::Engine& engine, bl::gui::rdr::Renderer&) {
    enginePtr   = &engine;
    auto& owner = getOwnerAs<EditMap>();

    renderTexture = engine.renderer().createRenderTexture(
        glm::u32vec2(core::Properties::WindowWidth(), core::Properties::WindowHeight()));
    sprite.create(engine, renderTexture->getTexture());
}

void EditMapComponent::doSceneAdd(bl::rc::Overlay* overlay) {
    sprite.addToScene(overlay, bl::rc::UpdateSpeed::Static);
}

void EditMapComponent::doSceneRemove() { sprite.removeFromScene(); }

void EditMapComponent::handleAcquisition() {
    EditMap& owner = getOwnerAs<EditMap>();

    // only respond to valid acquisition
    const glm::u32vec2 acq(owner.getAcquisition().width, owner.getAcquisition().height);
    if (acq.x > 0 && acq.x < 5000 && acq.y > 0 && acq.y < 5000) {
        if (acq != renderTexture->getSize()) {
            renderTexture->resize(acq);
            sprite.scaleToSize(acq);
        }
        handleMove();
    }
}

void EditMapComponent::handleMove() {
    EditMap& owner = getOwnerAs<EditMap>();
    sprite.getTransform().setPosition(owner.getLocalPosition().x, owner.getLocalPosition().y);
}

} // namespace rdr
} // namespace component
} // namespace editor
