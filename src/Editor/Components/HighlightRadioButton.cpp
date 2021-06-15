#include <Editor/Components/HighlightRadioButton.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;
HighlightRadioButton::Ptr HighlightRadioButton::create(Element::Ptr child,
                                                       RadioButton::Group* group) {
    Ptr r(new HighlightRadioButton(child, group));
    r->finishCreate();
    return r;
}

HighlightRadioButton::HighlightRadioButton(Element::Ptr c, RadioButton::Group* g)
: RadioButton(c, g, "", "") {
    highlight.setFillColor(sf::Color(252, 252, 23, 128));
}

void HighlightRadioButton::onAcquisition() {
    ToggleButton::onAcquisition();
    highlight.setPosition(getAcquisition().left, getAcquisition().top);
    highlight.setSize(
        {static_cast<float>(getAcquisition().width), static_cast<float>(getAcquisition().height)});
}

void HighlightRadioButton::doRender(sf::RenderTarget& target, sf::RenderStates states,
                                    const Renderer& renderer) const {
    ToggleButton::doRender(target, states, renderer);
    if (getValue()) target.draw(highlight, states);
}

} // namespace component
} // namespace editor
