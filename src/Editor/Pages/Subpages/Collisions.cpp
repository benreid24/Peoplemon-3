#include <Editor/Pages/Subpages/Collisions.hpp>

#include <Core/Resources.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>
#include <type_traits>

namespace editor
{
namespace page
{
using namespace bl::gui;

namespace
{
using C = core::map::Collision;

const std::pair<std::string, core::map::Collision> source[] = {
    {"EditorResources/Collisions/all.png", C::Open},
    {"EditorResources/Collisions/none.png", C::Blocked},
    {"EditorResources/Collisions/noTop.png", C::TopClosed},
    {"EditorResources/Collisions/noRight.png", C::RightClosed},
    {"EditorResources/Collisions/noBottom.png", C::BottomClosed},
    {"EditorResources/Collisions/noLeft.png", C::LeftClosed},
    {"EditorResources/Collisions/topRight.png", C::TopRightOpen},
    {"EditorResources/Collisions/bottomRight.png", C::BottomRightOpen},
    {"EditorResources/Collisions/bottomLeft.png", C::BottomLeftOpen},
    {"EditorResources/Collisions/topLeft.png", C::TopLeftOpen},
    {"EditorResources/Collisions/topBottom.png", C::TopBottomOpen},
    {"EditorResources/Collisions/leftRight.png", C::LeftRightOpen},
    {"EditorResources/Collisions/top.png", C::TopOpen},
    {"EditorResources/Collisions/right.png", C::RightOpen},
    {"EditorResources/Collisions/bottom.png", C::BottomOpen},
    {"EditorResources/Collisions/left.png", C::LeftOpen},
    {"EditorResources/Collisions/water.png", C::SurfRequired},
    {"EditorResources/Collisions/fall.png", C::WaterfallRequired},
    {"EditorResources/Collisions/ledge.png", C::LedgeHop}};
constexpr std::size_t nColTypes = std::extent<decltype(source)>::value;

} // namespace

Collisions::Collisions() {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 6));

    Box::Ptr row              = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    RadioButton::Group* group = nullptr;
    for (unsigned int i = 0; i < nColTypes; ++i) {
        const auto& pair = source[i];

        auto txtr      = ImageManager::load(pair.first);
        Image::Ptr img = Image::create(txtr);
        img->scaleToSize({64, 64});
        component::HighlightRadioButton::Ptr but =
            component::HighlightRadioButton::create(img, group);
        if (group == nullptr) but->setValue(true);
        but->getSignal(Event::LeftClicked).willAlwaysCall([this, &pair](const Event&, Element*) {
            active = pair.second;
        });
        group = but->getRadioGroup();
        row->pack(but);

        if ((i + 1) % 4 == 0) {
            content->pack(row, true, false);
            row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
        }
    }
    content->pack(row);
}

Element::Ptr Collisions::getContent() { return content; }

core::map::Collision Collisions::selected() const { return active; }

} // namespace page
} // namespace editor
