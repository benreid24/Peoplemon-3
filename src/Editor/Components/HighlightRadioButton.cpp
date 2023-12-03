#include <Editor/Components/HighlightRadioButton.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;
HighlightRadioButton::Ptr HighlightRadioButton::create(Element::Ptr child,
                                                       RadioButton::Group* group) {
    return Ptr(new HighlightRadioButton(child, group));
}

HighlightRadioButton::HighlightRadioButton(Element::Ptr c, RadioButton::Group* g)
: RadioButton(c, "", g) {
    highlight.setFillColor(sf::Color(252, 252, 23, 128));
}

void HighlightRadioButton::onAcquisition() {
    ToggleButton::onAcquisition();
    highlight.setSize({getAcquisition().width, getAcquisition().height});
}

bl::gui::rdr::Component* HighlightRadioButton::doPrepareRender(bl::gui::rdr::Renderer& renderer) {
    return nullptr;
}

// void HighlightRadioButton::doRender(sf::RenderTarget& target, sf::RenderStates states,
//                                     const Renderer& renderer) const {
//     ToggleButton::doRender(target, states, renderer);
//     if (getValue()) {
//         highlight.setPosition(getPosition());
//         target.draw(highlight, states);
//     }
// }

} // namespace component
} // namespace editor
