#include <Editor/Pages/Subpages/LevelTransitions.hpp>

#include <BLIB/Interfaces/Utilities.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>

namespace editor
{
namespace page
{
using core::map::LevelTransition;
using namespace bl::gui;
namespace
{
const std::pair<std::string, LevelTransition> source[] = {
    {"EditorResources/LevelTransitions/horUpLeft.png", LevelTransition::HorizontalRightDown},
    {"EditorResources/LevelTransitions/horUpRight.png", LevelTransition::HorizontalRightUp},
    {"EditorResources/LevelTransitions/vertUpDown.png", LevelTransition::VerticalTopDown},
    {"EditorResources/LevelTransitions/vertUpUp.png", LevelTransition::VerticalTopUp}};
const std::size_t sourceN = std::size(source);
} // namespace

LevelTransitions::LevelTransitions() {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 8.f));

    Label::Ptr help =
        Label::create("Walking in the direction of the arrow on a transition makes the "
                      "player go up a level. Walking opposite the arrow makes the player "
                      "go down a level. Walking perpendicular has no effect.");
    help->setTextWrapping(bl::gui::Label::WrapToAcquisition);
    content->pack(help, true, false);

    RadioButton::Ptr noneBut = RadioButton::create("Flat", "flat");
    noneBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&LevelTransitions::update, this, LevelTransition::None));
    content->pack(noneBut);

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->setHorizontalAlignment(RenderSettings::Left);

    RadioButton::Group* group = noneBut->getRadioGroup();
    for (unsigned int i = 0; i < sourceN; ++i) {
        const auto& pair = source[i];
        auto txtr        = ImageManager::load(pair.first);
        Image::Ptr img   = Image::create(txtr);
        img->scaleToSize({64, 64});
        component::HighlightRadioButton::Ptr but =
            component::HighlightRadioButton::create(img, group);
        but->getSignal(Event::LeftClicked)
            .willAlwaysCall(std::bind(&LevelTransitions::update, this, pair.second));
        row->pack(but, false, true);
        if (i == 1) {
            content->pack(row, true, false);
            row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
            row->setHorizontalAlignment(RenderSettings::Left);
        }
    }
    content->pack(row, true, false);
}

Element::Ptr LevelTransitions::getContent() { return content; }

LevelTransition LevelTransitions::getActive() const { return active; }

void LevelTransitions::update(LevelTransition lt) { active = lt; }

} // namespace page
} // namespace editor
