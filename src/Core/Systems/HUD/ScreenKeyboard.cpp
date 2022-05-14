#include <Core/Systems/HUD/ScreenKeyboard.hpp>

#include <BLIB/Engine.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace system
{
namespace hud
{
namespace
{
constexpr float InputAreaHeight = 35.f;
}

using namespace bl::menu;

ScreenKeyboard::ScreenKeyboard(const OnSubmit& os)
: onSubmit(os)
, minLen(0)
, maxLen(16)
, keyboardMenu(ArrowSelector::create(8.f, sf::Color::Black)) {
    renderedInput.setFont(Properties::MenuFont());
    renderedInput.setPosition({15.f, 5.f});
    renderedInput.setFillColor(sf::Color::Black);
    renderedInput.setCharacterSize(28);

    Item::Ptr space = TextItem::create("(space)", Properties::MenuFont(), sf::Color::Black, 24);
    space->getSignal(Item::EventType::Activated).willAlwaysCall([this]() {
        input.push_back(' ');
        renderedInput.setString(input);
    });

    Item::Ptr del = TextItem::create("(del)", Properties::MenuFont(), sf::Color::Red, 24);
    space->getSignal(Item::EventType::Activated).willAlwaysCall([this]() {
        input.pop_back();
        renderedInput.setString(input);
    });

    Item::Ptr submit = TextItem::create("Submit", Properties::MenuFont(), sf::Color::Green, 24);
    submit->getSignal(Item::EventType::Activated)
        .willAlwaysCall(std::bind(&ScreenKeyboard::onEnter, this));

    std::vector<std::vector<TextItem::Ptr>> keys;
    keys.resize(4);
    for (unsigned int i = 0; i < 4; ++i) { keys[i].reserve(static_cast<std::size_t>(13)); }

    const auto createKey = [this, &keys](unsigned int i, char k) {
        keys[i].emplace_back(
            TextItem::create(std::string(1, k), Properties::MenuFont(), sf::Color::Black, 24));
        keys[i]
            .back()
            ->getSignal(Item::EventType::Activated)
            .willAlwaysCall(std::bind(&ScreenKeyboard::onKeyPress, this, k));
    };

    for (char l = 'A'; l <= 'M'; ++l) { createKey(0, l); }
    for (char l = 'N'; l <= 'Z'; ++l) { createKey(1, l); }
    for (char l = 'a'; l <= 'm'; ++l) { createKey(2, l); }
    for (char l = 'n'; l <= 'z'; ++l) { createKey(3, l); }

    Item* topRow[3] = {space.get(), del.get(), submit.get()};
    keyboardMenu.setPosition({3.f, InputAreaHeight - 6.f});
    keyboardMenu.setMinWidth(28.f);
    keyboardMenu.setMinHeight(30.f);
    keyboardMenu.setPadding({18.f, 4.f});
    keyboardMenu.setRootItem(space);
    keyboardMenu.addItem(del, space.get(), Item::Right);
    keyboardMenu.addItem(submit, del.get(), Item::Right);

    keyboardMenu.addItem(keys[0][0], space.get(), Item::Bottom);
    for (unsigned int i = 1; i < keys[0].size(); ++i) {
        keyboardMenu.addItem(keys[0][i], keys[0][i - 1].get(), Item::Right);
        if (i < 3) { keyboardMenu.attachExisting(keys[0][i].get(), topRow[i], Item::Bottom); }
        else {
            keyboardMenu.attachExisting(submit.get(), keys[0][i].get(), Item::Top, false);
        }
    }
    keyboardMenu.attachExisting(keys[0].back().get(), keys[0].front().get(), Item::Left);

    for (unsigned int i = 1; i < 4; ++i) {
        for (unsigned int j = 0; j < keys[i].size(); ++j) {
            if (j < keys[i - 1].size()) {
                keyboardMenu.addItem(keys[i][j], keys[i - 1][j].get(), Item::Bottom);
            }
            else {
                keyboardMenu.addItem(keys[i][j], keys[i][j - 1].get(), Item::Right);
            }
            if (j > 0) {
                keyboardMenu.attachExisting(keys[i][j].get(), keys[i][j - 1].get(), Item::Right);
            }
        }
        keyboardMenu.attachExisting(keys[i].back().get(), keys[i].front().get(), Item::Left);
    }

    for (unsigned int i = 0; i < keys.back().size(); ++i) {
        if (i < 3) { keyboardMenu.attachExisting(keys.back()[i].get(), topRow[i], Item::Top); }
        else {
            keyboardMenu.attachExisting(submit.get(), keys.back()[i].get(), Item::Bottom, false);
        }
    }
    keyboardMenu.setSelectedItem(keys[0][0].get());

    background.setSize({keyboardMenu.getBounds().width + 28.f,
                        keyboardMenu.getBounds().height + InputAreaHeight + 20.f});
    background.setFillColor(sf::Color::White);
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(2.5f);
}

ScreenKeyboard::~ScreenKeyboard() { stop(); }

void ScreenKeyboard::start(unsigned int mn, unsigned int mx) {
    minLen = mn;
    maxLen = mx;
    input.clear();
    renderedInput.setString(input);
    inputDriver.drive(&keyboardMenu);
}

void ScreenKeyboard::stop() { inputDriver.drive(nullptr); }

void ScreenKeyboard::process(component::Command cmd) { inputDriver.process(cmd); }

void ScreenKeyboard::setPosition(const sf::Vector2f& pos) { position = pos; }

const std::string& ScreenKeyboard::value() const { return input; }

const sf::Vector2f& ScreenKeyboard::getSize() const { return background.getSize(); }

void ScreenKeyboard::onKeyPress(char c) {
    input.push_back(c);
    renderedInput.setString(input);
}

void ScreenKeyboard::onEnter() {
    if (input.size() >= minLen && input.size() <= maxLen) { onSubmit(input); }
}

void ScreenKeyboard::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform.translate(position);
    target.draw(background, states);
    target.draw(renderedInput, states);
    keyboardMenu.render(target, states);
}

} // namespace hud
} // namespace system
} // namespace core
