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

    std::vector<std::vector<Item::Ptr>> keys;
    keys.resize(4);
    for (unsigned int i = 0; i < 4; ++i) { keys[i].reserve(static_cast<std::size_t>(13)); }

    for (char l = 'A'; l != 'M'; ++l) {
        keys[0].emplace_back(
            TextItem::create(std::string(1, l), Properties::MenuFont(), sf::Color::Black, 24));
        keys[0]
            .back()
            ->getSignal(Item::EventType::Activated)
            .willAlwaysCall(std::bind(&ScreenKeyboard::onKeyPress, this, l));
    }
    for (char l = 'N'; l != 'Z'; ++l) {
        keys[1].emplace_back(
            TextItem::create(std::string(1, l), Properties::MenuFont(), sf::Color::Black, 24));
        keys[1]
            .back()
            ->getSignal(Item::EventType::Activated)
            .willAlwaysCall(std::bind(&ScreenKeyboard::onKeyPress, this, l));
    }
    for (char l = 'a'; l != 'm'; ++l) {
        keys[2].emplace_back(
            TextItem::create(std::string(1, l), Properties::MenuFont(), sf::Color::Black, 24));
        keys[2]
            .back()
            ->getSignal(Item::EventType::Activated)
            .willAlwaysCall(std::bind(&ScreenKeyboard::onKeyPress, this, l));
    }
    for (char l = 'n'; l != 'z'; ++l) {
        keys[3].emplace_back(
            TextItem::create(std::string(1, l), Properties::MenuFont(), sf::Color::Black, 24));
        keys[3]
            .back()
            ->getSignal(Item::EventType::Activated)
            .willAlwaysCall(std::bind(&ScreenKeyboard::onKeyPress, this, l));
    }

    Item* topRow[4] = {kbSwitch.get(), space.get(), del.get(), submit.get()};
    keyboardMenu.setRootItem(kbSwitch);
    keyboardMenu.addItem(space, kbSwitch.get(), Item::Right);
    keyboardMenu.addItem(del, space.get(), Item::Right);
    keyboardMenu.addItem(submit, del.get(), Item::Right);

    keyboardMenu.addItem(keys[0][0], kbSwitch.get(), Item::Bottom);
    for (unsigned int i = 1; i < keys[0].size(); ++i) {
        keyboardMenu.addItem(keys[0][i], keys[0][i - 1].get(), Item::Right);
        if (i < 4) { keyboardMenu.attachExisting(keys[0][i].get(), topRow[i], Item::Bottom); }
        else {
            keyboardMenu.attachExisting(submit.get(), keys[0][i].get(), Item::Top, false);
        }
    }
    keyboardMenu.attachExisting(keys[0].back().get(), keys[0].front().get(), Item::Left);

    for (unsigned int i = 1; i < 4; ++i) {
        for (unsigned int j = 0; j < keys[i].size(); ++j) {
            if (keys[i - 1].size()) {
                keyboardMenu.addItem(keys[i][j], keys[i - 1][j].get(), Item::Bottom);
            }
            else {
                keyboardMenu.addItem(keys[i][j], keys[i][j - 1].get(), Item::Right);
            }
        }
        keyboardMenu.attachExisting(keys[i].back().get(), keys[i].front().get(), Item::Left);
    }

    for (unsigned int i = 0; i < keys.back().size(); ++i) {
        if (i < 4) { keyboardMenu.attachExisting(keys.back()[i].get(), topRow[i], Item::Top); }
        else {
            keyboardMenu.attachExisting(submit.get(), keys.back()[i].get(), Item::Bottom, false);
        }
    }
}

ScreenKeyboard::~ScreenKeyboard() { stop(); }

void ScreenKeyboard::start() { bus.subscribe(this); }

void ScreenKeyboard::stop() { bus.unsubscribe(this); }

void ScreenKeyboard::setPosition(const sf::Vector2f& pos) { position = pos; }

const std::string& ScreenKeyboard::value() const { return input; }

void ScreenKeyboard::observe(const sf::Event& e) {
    // TODO - get text entered events and paste events
}

void ScreenKeyboard::onKeyPress(char c) {
    input.push_back(c);
    renderedInput.setString(input);
}

void ScreenKeyboard::onKeyboardToggle() {
    keyboardEnabled = !keyboardEnabled;
    if (keyboardEnabled) { kbSwitch->setTexture(enableKbTxtr); }
    else {
        kbSwitch->setTexture(disableKbTxtr);
    }
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

} // namespace menu
} // namespace core
