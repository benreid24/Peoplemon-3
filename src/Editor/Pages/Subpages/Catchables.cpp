#include <Editor/Pages/Subpages/Catchables.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>

namespace editor
{
namespace page
{
namespace
{
const std::array<sf::Color, 10> colors = {sf::Color(89, 147, 240, 150),
                                          sf::Color(89, 237, 240, 150),
                                          sf::Color(89, 240, 164, 150),
                                          sf::Color(139, 240, 89, 150),
                                          sf::Color(227, 240, 89, 150),
                                          sf::Color(240, 200, 89, 150),
                                          sf::Color(240, 144, 89, 150),
                                          sf::Color(240, 89, 119, 150),
                                          sf::Color(240, 89, 237, 150),
                                          sf::Color(170, 89, 240, 150)};
}

using namespace bl::gui;

Catchables::Catchables() {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 10));

    auto txtr = bl::engine::Resources::textures().load("EditorResources/Collisions/none.png").data;
    Image::Ptr img = Image::create(txtr);
    img->scaleToSize({128, 128});
    component::HighlightRadioButton::Ptr noCatch = component::HighlightRadioButton::create(img);
    noCatch->setValue(true);
    noCatch->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        active = 0;
    });

    txtr = bl::engine::Resources::textures().load("EditorResources/Collisions/all.png").data;
    img  = Image::create(txtr);
    img->scaleToSize({128, 128});
    component::HighlightRadioButton::Ptr catchPossible =
        component::HighlightRadioButton::create(img, noCatch->getRadioGroup());
    catchPossible->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        active = 1; // TODO - need to get the actual regions from the map
    });

    content->pack(noCatch, true, true);
    content->pack(catchPossible, true, true);
}

Element::Ptr Catchables::getContent() { return content; }

std::uint8_t Catchables::selected() const { return active; }

sf::Color Catchables::getColor(std::uint8_t i) {
    return i > 0 ? colors[i % colors.size()] : sf::Color::Transparent;
}

} // namespace page
} // namespace editor
