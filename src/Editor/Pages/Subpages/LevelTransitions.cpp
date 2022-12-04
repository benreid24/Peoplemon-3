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
    content = Box::create(LinePacker::create(LinePacker::Vertical, 6));

    Label::Ptr help =
        Label::create("Walking in the direction of the arrow on a transition makes the "
                      "player go up a level. Walking opposite the arrow makes the player "
                      "go down a level. Walking perpendicular has no effect.");
    sf::Text text;
    text.setFont(core::Properties::MenuFont());
    text.setCharacterSize(18);
    text.setString(help->getText());
    bl::interface::wordWrap(text, 400.f);
    help->setText(text.getString());
    help->setCharacterSize(text.getCharacterSize());
    content->pack(help);

    RadioButton::Ptr noneBut = RadioButton::create("Flat", "flat");
    noneBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&LevelTransitions::update, this, LevelTransition::None));
    content->pack(noneBut);

    Box::Ptr row              = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    RadioButton::Group* group = noneBut->getRadioGroup();
    for (unsigned int i = 0; i < sourceN; ++i) {
        const auto& pair = source[i];
        auto txtr        = TextureManager::load(pair.first).data;
        Image::Ptr img   = Image::create(txtr);
        img->scaleToSize({64, 64});
        component::HighlightRadioButton::Ptr but =
            component::HighlightRadioButton::create(img, group);
        but->getSignal(Event::LeftClicked)
            .willAlwaysCall(std::bind(&LevelTransitions::update, this, pair.second));
        row->pack(but);
        if (i == 1) {
            content->pack(row);
            row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
        }
    }
    content->pack(row);
}

Element::Ptr LevelTransitions::getContent() { return content; }

LevelTransition LevelTransitions::getActive() const { return active; }

void LevelTransitions::update(LevelTransition lt) { active = lt; }

} // namespace page
} // namespace editor
