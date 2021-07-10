#include <Editor/Pages/Subpages/Catchables.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Catchables::Catchables() {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 10));

    auto txtr = bl::engine::Resources::textures().load("EditorResources/Collisions/none.png").data;
    Image::Ptr img = Image::create(txtr);
    img->scaleToSize({128, 128});
    component::HighlightRadioButton::Ptr noCatch = component::HighlightRadioButton::create(img);
    noCatch->setValue(true);
    noCatch->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        active = core::map::Catch::NoEncounter;
    });

    txtr = bl::engine::Resources::textures().load("EditorResources/Collisions/all.png").data;
    img  = Image::create(txtr);
    img->scaleToSize({128, 128});
    component::HighlightRadioButton::Ptr catchPossible =
        component::HighlightRadioButton::create(img, noCatch->getRadioGroup());
    catchPossible->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        active = core::map::Catch::RandomEncounter;
    });

    content->pack(noCatch, true, true);
    content->pack(catchPossible, true, true);
}

Element::Ptr Catchables::getContent() { return content; }

core::map::Catch Catchables::selected() const { return active; }

} // namespace page
} // namespace editor
