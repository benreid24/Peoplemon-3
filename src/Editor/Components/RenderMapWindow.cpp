#include <Editor/Components/RenderMapWindow.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

RenderMapWindow::RenderMapWindow(const StartRender& os)
: trigger(os) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Render Map");
    window->getSignal(Event::Closed).willAlwaysCall(std::bind(&RenderMapWindow::close, this));

    window->pack(Label::create("Render the map to an image file"));

    charBut = CheckButton::create("Render characters");
    window->pack(charBut);
    lightSelect = LightSlider::create("Light Level", []() {});
    window->pack(lightSelect);

    Box::Ptr row          = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    Button::Ptr renderBut = Button::create("Render");
    renderBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&RenderMapWindow::start, this));
    row->pack(renderBut);
    Button::Ptr closeBut = Button::create("Cancel");
    closeBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&RenderMapWindow::close, this));
    window->pack(row);
}

void RenderMapWindow::open(bl::gui::GUI* gui) {
    gui->pack(window);
    window->setForceFocus(true);
    lightSelect->setLightLevel(255);
}

const std::string& RenderMapWindow::outputPath() const { return output; }

bool RenderMapWindow::renderCharacters() const { return charBut->getValue(); }

std::uint8_t RenderMapWindow::lightLevel() const { return lightSelect->getLightLevel(); }

void RenderMapWindow::close() {
    window->setForceFocus(false);
    window->remove();
}

void RenderMapWindow::start() {
    const char* filters[] = {"*.png"};
    char* path =
        bl::dialog::tinyfd_saveFileDialog("Save Map Rendering", "", 1, filters, "PNG files");
    if (path) {
        output = path;
        close();
        trigger();
    }
}

} // namespace component
} // namespace editor
