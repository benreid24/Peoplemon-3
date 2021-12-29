#include <Game/States/PeoplemonMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr PeoplemonMenu::create(core::system::Systems& s, Context c, ContextData* d) {
    return Ptr(new PeoplemonMenu(s, c, d));
}

PeoplemonMenu::PeoplemonMenu(core::system::Systems& s, Context c, ContextData* d)
: State(s)
, context(c)
, data(d)
, state(Browsing)
, menu(bl::menu::NoSelector::create()) {
    backgroundTxtr = bl::engine::Resources::textures()
                         .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                            "Peoplemon/background.png"))
                         .data;
    background.setTexture(*backgroundTxtr, true);

    const sf::Vector2f MenuPosition(41.f, 5.f);
    menu::PeoplemonButton::Ptr ppl = menu::PeoplemonButton::create(systems.player().team().front());
    menu.setRootItem(ppl);
    menu.setPosition(MenuPosition);

    backBut = bl::menu::ImageItem::create(
        bl::engine::Resources::textures()
            .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                               "Peoplemon/cancel.png"))
            .data);
    backBut->overridePosition(
        sf::Vector2f(core::Properties::WindowWidth(), core::Properties::WindowHeight()) -
        backBut->getSize() - MenuPosition - sf::Vector2f(10.f, 5.f));
    backBut->getSignal(bl::menu::Item::Selected).willAlwaysCall([this]() {
        backBut->getSprite().setColor(sf::Color(180, 100, 80));
    });
    backBut->getSignal(bl::menu::Item::Deselected).willAlwaysCall([this]() {
        backBut->getSprite().setColor(sf::Color::White);
    });
}

const char* PeoplemonMenu::name() const { return "PeoplemonMenu"; }

void PeoplemonMenu::activate(bl::engine::Engine& engine) {
    oldView       = engine.window().getView();
    sf::View view = oldView;
    view.setCenter(background.getGlobalBounds().width * 0.5f,
                   background.getGlobalBounds().height * 0.5f);
    view.setSize(background.getGlobalBounds().width, background.getGlobalBounds().height);
    engine.window().setView(view);

    for (unsigned int i = 0; i < 6; ++i) { buttons[i].reset(); }

    const auto& team         = systems.player().team();
    const unsigned int col1N = team.size() / 2 + team.size() % 2;
    const unsigned int col2N = team.size() / 2;
    for (unsigned int i = 0; i < col1N; ++i) {
        buttons[i * 2] = menu::PeoplemonButton::create(team[i * 2]);
    }
    for (unsigned int i = 0; i < col2N; ++i) {
        buttons[i * 2 + 1] = menu::PeoplemonButton::create(team[i * 2 + 1]);
    }
    menu.setRootItem(buttons[0]);
    for (unsigned int i = 1; i < col1N; ++i) {
        menu.addItem(buttons[i * 2], buttons[(i - 1) * 2].get(), bl::menu::Item::Bottom);
    }
    for (unsigned int i = 0; i < col2N; ++i) {
        menu.addItem(buttons[i * 2 + 1], buttons[i * 2].get(), bl::menu::Item::Right);
        if (i > 0) {
            menu.attachExisting(
                buttons[i * 2 + 1].get(), buttons[(i - 1) * 2 + 1].get(), bl::menu::Item::Bottom);
        }
    }
    if (col2N > 0) {
        menu.addItem(backBut, buttons[(col1N - 1) * 2 + 1].get(), bl::menu::Item::Bottom);
        menu.attachExisting(backBut.get(), buttons[(col1N - 1) * 2].get(), bl::menu::Item::Bottom, false);
    }
    else {
        menu.addItem(backBut, buttons[(col1N - 1) * 2].get(), bl::menu::Item::Bottom);
    }
    menu.setSelectedItem(buttons[0].get());

    inputDriver.drive(&menu);
    systems.player().inputSystem().addListener(inputDriver);
}

void PeoplemonMenu::deactivate(bl::engine::Engine& engine) {
    inputDriver.drive(nullptr);
    systems.player().inputSystem().removeListener(inputDriver);
    engine.window().setView(oldView);
}

void PeoplemonMenu::update(bl::engine::Engine&, float) {
    systems.player().update();
    if (inputDriver.backPressed()) {
        if (context != Context::BattleFaint) { systems.engine().popState(); }
    }
}

void PeoplemonMenu::render(bl::engine::Engine& engine, float) {
    engine.window().clear();
    engine.window().draw(background);
    menu.render(engine.window());
    engine.window().display();
}

} // namespace state
} // namespace game
