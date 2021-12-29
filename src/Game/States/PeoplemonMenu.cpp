#include <Game/States/PeoplemonMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Menus/PeoplemonButton.hpp>
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
, menu(bl::menu::NoSelector::create()) {
    backgroundTxtr = bl::engine::Resources::textures()
                         .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                            "Peoplemon/background.png"))
                         .data;
    background.setTexture(*backgroundTxtr, true);

    core::menu::PeoplemonButton::Ptr ppl =
        core::menu::PeoplemonButton::create(systems.player().team().front());
    menu.setRootItem(ppl);
    menu.setPosition({50.f, 150.f});
}

const char* PeoplemonMenu::name() const { return "PeoplemonMenu"; }

void PeoplemonMenu::activate(bl::engine::Engine& engine) {
    oldView       = engine.window().getView();
    sf::View view = oldView;
    view.setCenter(background.getGlobalBounds().width * 0.5f,
                   background.getGlobalBounds().height * 0.5f);
    view.setSize(background.getGlobalBounds().width, background.getGlobalBounds().height);
    engine.window().setView(view);

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
