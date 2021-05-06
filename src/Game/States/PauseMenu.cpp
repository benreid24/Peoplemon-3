#include <Game/States/PauseMenu.hpp>

#include <Core/Properties.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float Width  = 200.f;
constexpr float Height = 400.f;
} // namespace

bl::engine::State::Ptr PauseMenu::create(core::system::Systems& systems) {
    return bl::engine::State::Ptr(new PauseMenu(systems));
}

PauseMenu::PauseMenu(core::system::Systems& s)
: State(s) {
    using bl::menu::Item;
    using bl::menu::TextRenderItem;

    sf::Text text;
    text.setFont(core::Properties::MenuFont());
    text.setFillColor(sf::Color::Black);
    text.setCharacterSize(30);

    text.setString("Resume");
    resume = Item::create(TextRenderItem::create(text));
    resume->getSignal(Item::Activated).willCall([this]() { this->systems.engine().popState(); });

    text.setString("Peopledex");
    ppldex = Item::create(TextRenderItem::create(text));
    ppldex->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Peopledex"; });

    text.setString("Peoplemon");
    pplmon = Item::create(TextRenderItem::create(text));
    pplmon->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Peoplemon"; });

    text.setString("Bag");
    bag = Item::create(TextRenderItem::create(text));
    bag->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Bag"; });

    text.setString("Map");
    map = Item::create(TextRenderItem::create(text));
    map->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Map"; });

    text.setString("Save");
    save = Item::create(TextRenderItem::create(text));
    save->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Save"; });

    text.setString("Settings");
    settings = Item::create(TextRenderItem::create(text));
    settings->getSignal(Item::Activated).willCall([]() { BL_LOG_INFO << "Settings"; });

    text.setString("Quit");
    quit = Item::create(TextRenderItem::create(text));
    quit->getSignal(Item::Activated).willCall([this]() {
        this->systems.engine().flags().set(bl::engine::Flags::Terminate);
    });

    resume->attach(ppldex, Item::Bottom);
    ppldex->attach(pplmon, Item::Bottom);
    pplmon->attach(bag, Item::Bottom);
    bag->attach(map, Item::Bottom);
    map->attach(save, Item::Bottom);
    save->attach(settings, Item::Bottom);
    settings->attach(quit, Item::Bottom);

    selector = bl::menu::ArrowSelector::create(14.f);
    selector->getArrow().setFillColor(sf::Color::Black);
    menu.emplace(resume, selector);
    inputDriver.drive(menu.get());
    renderer.setVerticalPadding(8.f);

    menuBackground.setSize({Width, Height});
    menuBackground.setFillColor(sf::Color::White);
    menuBackground.setOutlineColor(sf::Color::Black);
    menuBackground.setOutlineThickness(3.f);
    menuBackground.setOrigin(0.f, 0.f);
    menuBackground.setPosition(0.f, 0.f);

    menuView.setCenter(menuBackground.getSize() / 2.f);
    menuView.setSize(menuBackground.getSize() +
                     sf::Vector2f(menuBackground.getOutlineThickness() * 2.f,
                                  menuBackground.getOutlineThickness() * 2.f));
}

const char* PauseMenu::name() const { return "PauseMenu"; }

void PauseMenu::activate(bl::engine::Engine&) {
    menu.get().setSelectedItem(resume);
    systems.player().inputSystem().addListener(inputDriver);
}

void PauseMenu::deactivate(bl::engine::Engine&) {
    systems.player().inputSystem().removeListener(inputDriver);
}

void PauseMenu::update(bl::engine::Engine&, float dt) {
    systems.player().update();
    systems.world().update(dt);
}

void PauseMenu::render(bl::engine::Engine&, float lag) {
    systems.render().render(systems.engine().window(), lag);

    const sf::View oldView = systems.engine().window().getView();
    const float w = menuView.getSize().x / systems.engine().window().getDefaultView().getSize().x *
                    oldView.getViewport().width;
    const float h = menuView.getSize().y / systems.engine().window().getDefaultView().getSize().y *
                    oldView.getViewport().height;
    menuView.setViewport({oldView.getViewport().left + oldView.getViewport().width - w * 1.15f,
                          oldView.getViewport().top + oldView.getViewport().height / 2.f - h / 2.f,
                          w,
                          h});
    systems.engine().window().setView(menuView);

    systems.engine().window().draw(menuBackground);
    menu.get().render(renderer, systems.engine().window(), {24.f, 0.f});
    systems.engine().window().display();

    systems.engine().window().setView(oldView);
}

} // namespace state
} // namespace game
