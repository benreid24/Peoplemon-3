#include <Editor/Components/LightSlider.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

LightSlider::Ptr LightSlider::create(const std::string& prompt, const ChangeCb& cb) {
    return Ptr(new LightSlider(prompt, cb));
}

LightSlider::LightSlider(const std::string& p, const ChangeCb& cb)
: Box(LinePacker::create(LinePacker::Horizontal, 4.f)) {
    Box::Ptr col = Box::create(LinePacker::create(LinePacker::Vertical, 2.f));
    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 0.f));
    row->pack(Label::create("Dark"), false, true);
    prompt = Label::create(p);
    row->pack(prompt, true, true);
    row->pack(Label::create("Bright"));
    col->pack(row, true, false);

    slider = Slider::create(Slider::Horizontal);
    slider->setRequisition({125.f, 20.f});
    slider->getSignal(Event::ValueChanged).willAlwaysCall(std::bind(cb));
    slider->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        preview->setLightLevel(getLightLevel());
    });
    col->pack(slider, true, false);
    pack(col, true, false);

    preview = LightPreview::create({80.f, 80.f});
    pack(preview);
}

std::uint8_t LightSlider::getLightLevel() const {
    return static_cast<std::uint8_t>(slider->getValue() * 255.f);
}

void LightSlider::setLightLevel(std::uint8_t level) {
    slider->setValue(static_cast<float>(level) / 255.f, false);
    preview->setLightLevel(level);
}

void LightSlider::setPrompt(const std::string& p) { prompt->setText(p); }

} // namespace component
} // namespace editor
