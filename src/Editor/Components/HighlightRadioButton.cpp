#include <Editor/Components/HighlightRadioButton.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

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
    setToggleSize(18.f);
}

void HighlightRadioButton::onAcquisition() { ToggleButton::onAcquisition(); }

bl::gui::rdr::Component* HighlightRadioButton::doPrepareRender(bl::gui::rdr::Renderer& renderer) {
    return renderer.createComponent<HighlightRadioButton>(*this);
}

} // namespace component
} // namespace editor
