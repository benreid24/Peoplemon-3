#include <Core/Menus/ScreenKeyboard.hpp>

#include <BLIB/Engine.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace menu
{
namespace
{
constexpr std::initializer_list<std::initializer_list<char>> KeyChars = {{}};
}

using namespace bl::menu;

ScreenKeyboard::ScreenKeyboard(bl::event::Dispatcher& b, const OnSubmit& os, unsigned int mn,
                               unsigned int mx)
: onSubmit(os)
, minLen(mn)
, maxLen(mx)
, bus(b)
, keyboardEnabled(false)
, position(100.f, 380.f)
, selector(ArrowSelector::create(8.f))
, keyboardMenu(selector) {
    enableKbTxtr =
        bl::engine::Resources::textures().load("Resources/Images/Menus/screenKbEnable.png").data;
    disableKbTxtr =
        bl::engine::Resources::textures().load("Resources/Images/Menus/screenKbDisable.png").data;
    kbSwitch = ImageItem::create(enableKbTxtr);
    kbSwitch->getSignal(Item::EventType::Activated)
        .willAlwaysCall(std::bind(&ScreenKeyboard::onKeyboardToggle, this));

    bgndTxtr = bl::engine::Resources::textures().load("Resources/Images/Menus/keyboard.png").data;
    background.setTexture(*bgndTxtr, true);
    renderedInput.setFont(Properties::MenuFont());
    renderedInput.setPosition({10.f, 5.f});
    renderedInput.setFillColor(sf::Color::Black);
    renderedInput.setCharacterSize(28);

    std::vector<std::vector<Item::Ptr>> keys;
    keys.resize(4);
    for (unsigned int i = 0; i < 4; ++i) { keys[i].reserve(static_cast<std::size_t>(13)); }

    Item::Ptr space = TextItem::create("<space>", Properties::MenuFont(), sf::Color::Black, 24);
    space->getSignal(Item::EventType::Activated).willAlwaysCall([this]() {
        input.push_back(' ');
        renderedInput.setString(input);
    });

    Item::Ptr del = TextItem::create("<del>", Properties::MenuFont(), sf::Color::Red, 24);
    space->getSignal(Item::EventType::Activated).willAlwaysCall([this]() {
        input.pop_back();
        renderedInput.setString(input);
    });

    Item::Ptr submit = TextItem::create("Submit", Properties::MenuFont(), sf::Color::Green, 24);
    submit->getSignal(Item::EventType::Activated)
        .willAlwaysCall(std::bind(&ScreenKeyboard::onEnter, this));
}

} // namespace menu
} // namespace core
