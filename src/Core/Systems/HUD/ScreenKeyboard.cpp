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

ScreenKeyboard::ScreenKeyboard(bl::engine::Engine& engine, const OnSubmit& os)
: engine(engine)
, onSubmit(os)
, minLen(0)
, maxLen(16)
, position() {}

void ScreenKeyboard::lazyCreate() {
    if (background.entity() != bl::ecs::InvalidEntity) { return; }

    keyboardMenu.create(
        engine, engine.renderer().getObserver(), ArrowSelector::create(8.f, sf::Color::Black));

    Item::Ptr space = TextItem::create("(space)", Properties::MenuFont(), sf::Color::Black, 24);
    space->getSignal(Item::EventType::Activated).willAlwaysCall([this]() {
        input.push_back(' ');
        renderedInput.getSection().setString(input);
    });

    Item::Ptr del = TextItem::create("(del)", Properties::MenuFont(), sf::Color::Red, 24);
    space->getSignal(Item::EventType::Activated).willAlwaysCall([this]() {
        input.pop_back();
        renderedInput.getSection().setString(input);
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
        else { keyboardMenu.attachExisting(submit.get(), keys[0][i].get(), Item::Top, false); }
    }
    keyboardMenu.attachExisting(keys[0].back().get(), keys[0].front().get(), Item::Left);

    for (unsigned int i = 1; i < 4; ++i) {
        for (unsigned int j = 0; j < keys[i].size(); ++j) {
            if (j < keys[i - 1].size()) {
                keyboardMenu.addItem(keys[i][j], keys[i - 1][j].get(), Item::Bottom);
            }
            else { keyboardMenu.addItem(keys[i][j], keys[i][j - 1].get(), Item::Right); }
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

    background.create(engine,
                      {keyboardMenu.getBounds().width + 28.f,
                       keyboardMenu.getBounds().height + InputAreaHeight + 20.f});
    background.setFillColor(sf::Color::White);
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(2.5f);
    background.getTransform().setPosition(position.x, position.y);
    engine.ecs().setEntityParentDestructionBehavior(
        background.entity(), bl::ecs::ParentDestructionBehavior::OrphanedByParent);

    renderedInput.create(engine, core::Properties::MenuFont(), "", 28, sf::Color::Black);
    renderedInput.setParent(background);
}

ScreenKeyboard::~ScreenKeyboard() { stop(); }

void ScreenKeyboard::start(unsigned int mn, unsigned int mx) {
    lazyCreate();

    minLen = mn;
    maxLen = mx;
    input.clear();
    renderedInput.getSection().setString(input);
    inputDriver.drive(&keyboardMenu);

    const auto overlay = engine.renderer().getObserver().getOrCreateSceneOverlay();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    renderedInput.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    keyboardMenu.addToOverlay(background.entity());
}

void ScreenKeyboard::stop() {
    inputDriver.drive(nullptr);
    if (background.entity() != bl::ecs::InvalidEntity) { background.removeFromScene(); }
}

void ScreenKeyboard::process(unsigned int ctrl, bool ignore) {
    inputDriver.sendControl(ctrl, ignore);
}

void ScreenKeyboard::setPosition(const sf::Vector2f& pos) {
    if (background.entity() != bl::ecs::InvalidEntity) {
        background.getTransform().setPosition(pos.x, pos.y);
    }
    else { position = pos; }
}

const std::string& ScreenKeyboard::value() const { return input; }

sf::Vector2f ScreenKeyboard::getSize() const {
    return {background.getSize().x, background.getSize().y};
}

void ScreenKeyboard::onKeyPress(char c) {
    input.push_back(c);
    renderedInput.getSection().setString(input);
}

void ScreenKeyboard::onEnter() {
    if (input.size() >= minLen && input.size() <= maxLen) { onSubmit(input); }
}

} // namespace hud
} // namespace system
} // namespace core
