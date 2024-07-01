#include <Editor/Components/Render/HighlightRadioButtonComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/RadioButton.hpp>

namespace editor
{
namespace component
{
namespace rdr
{
HighlightRadioButtonComponent::HighlightRadioButtonComponent()
: Component(Component::HighlightState::HighlightedByMouse) {}

void HighlightRadioButtonComponent::setVisible(bool visible) { base.setVisible(visible); }

void HighlightRadioButtonComponent::onElementUpdated() {
    base.onElementUpdated();
    highlight.setHidden(!getOwnerAs<bl::gui::RadioButton>().getValue());
}

void HighlightRadioButtonComponent::onRenderSettingChange() { base.onRenderSettingChange(); }

bl::ecs::Entity HighlightRadioButtonComponent::getEntity() const { return base.getEntity(); }

void HighlightRadioButtonComponent::doCreate(bl::engine::Engine& engine,
                                             bl::gui::rdr::Renderer& renderer) {
    createChild(base);
    highlight.create(engine, {100.f, 100.f});
    highlight.setFillColor(sf::Color(252, 252, 23, 128));
    highlight.setParent(base.getEntity());
}

void HighlightRadioButtonComponent::doSceneAdd(bl::rc::Overlay* overlay) {
    Component::doSceneAdd(base, overlay);
    highlight.addToScene(overlay, bl::rc::UpdateSpeed::Static);
}

void HighlightRadioButtonComponent::doSceneRemove() {
    Component::doSceneRemove(base);
    highlight.removeFromScene();
}

void HighlightRadioButtonComponent::handleAcquisition() {
    base.onAcquisition();

    const sf::FloatRect& acq = getOwnerAs<bl::gui::RadioButton>().getAcquisition();
    highlight.scaleToSize({acq.width, acq.height});
}

void HighlightRadioButtonComponent::handleMove() { base.onMove(); }

sf::Vector2f HighlightRadioButtonComponent::getRequisition() const { return base.getRequisition(); }

} // namespace rdr
} // namespace component
} // namespace editor
