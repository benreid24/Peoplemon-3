#include <Game/States/BagMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
bl::engine::State::Ptr BagMenu::create(core::system::Systems& s, Context c, core::item::Id* i) {
    return bl::engine::State::Ptr(new BagMenu(s, c, i));
}

BagMenu::BagMenu(core::system::Systems& s, Context c, core::item::Id* i)
: State(s)
, context(c)
, result(i) {
    bgndTxtr = bl::engine::Resources::textures()
                   .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                      "Bag/background.png"))
                   .data;
    background.setTexture(*bgndTxtr, true);
}

const char* BagMenu::name() const { return "BagMenu"; }

void BagMenu::activate(bl::engine::Engine& engine) {
    oldView = engine.window().getView();
    sf::View v(oldView);
    const sf::Vector2f size(background.getGlobalBounds().width,
                            background.getGlobalBounds().height);
    v.setCenter(size * 0.5f);
    v.setSize(size);
    engine.window().setView(v);

    systems.player().inputSystem().addListener(inputDriver);
    if (result) *result = core::item::Id::None;

    // TODO - initialize menus
}

void BagMenu::deactivate(bl::engine::Engine& engine) {
    engine.window().setView(oldView);
    systems.player().inputSystem().removeListener(inputDriver);
}

void BagMenu::update(bl::engine::Engine& engine, float) {
    systems.player().update();
    if (inputDriver.backPressed()) { engine.popState(); }
}

void BagMenu::render(bl::engine::Engine& engine, float) {
    engine.window().clear();
    engine.window().draw(background);
    engine.window().display();
}

} // namespace state
} // namespace game
