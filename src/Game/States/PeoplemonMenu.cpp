#include <Game/States/PeoplemonMenu.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float MoveTime = 0.85f;
}

bl::engine::State::Ptr PeoplemonMenu::create(core::system::Systems& s, Context c, ContextData* d) {
    return Ptr(new PeoplemonMenu(s, c, d));
}

PeoplemonMenu::PeoplemonMenu(core::system::Systems& s, Context c, ContextData* d)
: State(s)
, context(c)
, data(d)
, state(Browsing)
, menu(bl::menu::NoSelector::create())
, actionMenu(bl::menu::ArrowSelector::create(10.f, sf::Color::Black))
, actionOpen(false) {
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
    backBut->getSignal(bl::menu::Item::Activated).willAlwaysCall([this]() {
        systems.engine().popState();
    });

    if (context == Context::PauseMenu) {
        bl::menu::TextItem::Ptr info = bl::menu::TextItem::create(
            "Summary", core::Properties::MenuFont(), sf::Color::Black, 26);
        bl::menu::TextItem::Ptr move =
            bl::menu::TextItem::create("Move", core::Properties::MenuFont(), sf::Color::Black, 26);
        bl::menu::TextItem::Ptr item = bl::menu::TextItem::create(
            "Take Item", core::Properties::MenuFont(), sf::Color::Black, 26);
        bl::menu::TextItem::Ptr back =
            bl::menu::TextItem::create("Back", core::Properties::MenuFont(), sf::Color::Black, 26);

        info->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&PeoplemonMenu::showInfo, this));
        move->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&PeoplemonMenu::startMove, this));
        item->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&PeoplemonMenu::takeItem, this));
        back->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&PeoplemonMenu::resetAction, this));

        actionMenu.setRootItem(info);
        actionMenu.addItem(item, info.get(), bl::menu::Item::Bottom);
        actionMenu.addItem(move, info.get(), bl::menu::Item::Right);
        actionMenu.addItem(back, move.get(), bl::menu::Item::Bottom);
        actionMenu.attachExisting(back.get(), item.get(), bl::menu::Item::Right);
        actionRoot = info.get();
    }
    else {
        bl::menu::TextItem::Ptr sel =
            bl::menu::TextItem::create(context == Context::StorageSelect ? "Store" : "Switch",
                                       core::Properties::MenuFont(),
                                       sf::Color::Black,
                                       26);
        bl::menu::TextItem::Ptr back =
            bl::menu::TextItem::create("Back", core::Properties::MenuFont(), sf::Color::Black, 26);

        sel->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&PeoplemonMenu::chosen, this));
        back->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&PeoplemonMenu::resetAction, this));

        actionMenu.setRootItem(sel);
        actionMenu.addItem(back, sel.get(), bl::menu::Item::Right);
        actionRoot = back.get();
    }
    actionMenu.setMinHeight(36.f);
    actionMenu.setPadding({18.f, 12.f});
    actionMenu.configureBackground(sf::Color::White, sf::Color::Black, 3.f, {12.f, 0.f, 0.f, 2.f});
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
    for (unsigned int i = 0; i < 6; ++i) {
        if (team[i].currentHp() == 0) {
            buttons[i]->setHighlightColor(sf::Color(200, 10, 10));
            if (context == Context::BattleFaint || context == Context::BattleSwitch) {
                buttons[i]->setSelectable(false);
            }
        }
        else if (data && data->outNow == i) {
            if (context == Context::BattleSwitch) {
                buttons[i]->setHighlightColor(sf::Color(90, 110, 250));
                buttons[i]->setSelectable(false);
            }
        }
        buttons[i]
            ->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&PeoplemonMenu::selected, this, buttons[i].get()));
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
    if (context != Context::BattleFaint) {
        if (col2N > 0) {
            menu.addItem(backBut, buttons[(col1N - 1) * 2 + 1].get(), bl::menu::Item::Bottom);
            menu.attachExisting(
                backBut.get(), buttons[(col1N - 1) * 2].get(), bl::menu::Item::Bottom, false);
        }
        else {
            menu.addItem(backBut, buttons[(col1N - 1) * 2].get(), bl::menu::Item::Bottom);
        }
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

void PeoplemonMenu::update(bl::engine::Engine&, float dt) {
    systems.player().update();
    if (inputDriver.backPressed()) {
        if (state == MenuState::SelectingMove) { cleanupMove(false); }
        else if (context != Context::BattleFaint) {
            systems.engine().popState();
        }
    }

    if (state == MenuState::Moving) {
        const sf::Vector2f d = moveVel * dt;
        const float mx       = std::abs(std::max(d.x, 5.f));
        const float my       = std::abs(std::max(d.y, 5.f));
        buttons[mover1]->overridePosition(buttons[mover1]->getPosition() + d);
        buttons[mover2]->overridePosition(buttons[mover2]->getPosition() - d);
        if (std::abs(buttons[mover1]->getPosition().x - mover1Dest.x) <= mx &&
            std::abs(buttons[mover1]->getPosition().y - mover1Dest.y) < my) {
            cleanupMove(true);
        }
    }
    else if (state == MenuState::ShowingMessage) {
        systems.hud().update(dt);
    }
}

void PeoplemonMenu::render(bl::engine::Engine& engine, float lag) {
    engine.window().clear();
    engine.window().draw(background);
    menu.render(engine.window());
    if (actionOpen) { actionMenu.render(engine.window()); }
    if (state == MenuState::ShowingMessage) { systems.hud().render(engine.window(), lag); }
    engine.window().display();
}

void PeoplemonMenu::selected(menu::PeoplemonButton* b) {
    unsigned int i = 0;
    for (; i < 6; ++i) {
        if (buttons[i].get() == b) break;
    }
    if (buttons[i].get() != b) {
        BL_LOG_ERROR << "Invalid button pointer";
        return;
    }

    switch (state) {
    case Browsing:
        actionOpen = true;
        actionMenu.setPosition(
            buttons[i]->getPosition() +
            sf::Vector2f(menu.getBounds().left + 85.f, menu.getBounds().top + 45.f));
        actionMenu.setSelectedItem(actionRoot);
        inputDriver.drive(&actionMenu);
        mover1 = i;
        break;

    case SelectingMove:
        mover2     = i;
        mover1Dest = buttons[mover2]->getPosition();
        mover2Dest = buttons[mover1]->getPosition();
        moveVel    = (mover1Dest - mover2Dest) / MoveTime;
        state      = MenuState::Moving;
        inputDriver.drive(nullptr);
        actionOpen = false;
        break;

    case Moving:
    default:
        break;
    }
}

void PeoplemonMenu::showInfo() {
    BL_LOG_INFO << "show info";
    // TODO
}

void PeoplemonMenu::startMove() {
    if (systems.player().team().size() > 1) {
        state = MenuState::SelectingMove;
        inputDriver.drive(&menu);
        buttons[mover1]->setHighlightColor(sf::Color(40, 120, 230));

        if (mover1 % 2 == 0) {
            if (buttons[mover1 + 1]) { menu.setSelectedItem(buttons[mover1 + 1].get()); }
            else {
                menu.setSelectedItem(buttons[mover1 - 2].get());
            }
        }
        else {
            menu.setSelectedItem(buttons[mover1 - 1].get());
        }
    }
}

void PeoplemonMenu::cleanupMove(bool c) {
    buttons[mover1]->setHighlightColor(systems.player().team()[mover1].currentHp() > 0 ?
                                           sf::Color::White :
                                           sf::Color(200, 10, 10));
    state      = MenuState::Browsing;
    actionOpen = false;

    if (!c) {
        inputDriver.drive(&actionMenu);
        menu.setSelectedItem(buttons[mover1].get());
    }
    else {
        buttons[mover1]->overridePosition(mover1Dest);
        buttons[mover2]->overridePosition(mover2Dest);
        std::swap(buttons[mover1], buttons[mover2]);
        std::swap(systems.player().team()[mover1], systems.player().team()[mover2]);
        connectButtons();
        menu.setSelectedItem(buttons[mover2].get());
        inputDriver.drive(&menu);
    }
}

void PeoplemonMenu::takeItem() {
    auto& ppl              = systems.player().team()[mover1];
    const core::item::Id i = ppl.holdItem();
    if (i != core::item::Id::None) {
        systems.player().bag().addItem(i);
        ppl.holdItem() = core::item::Id::None;
        systems.hud().displayMessage(ppl.name() + " had its " + core::item::Item::getName(i) +
                                         " taken away",
                                     std::bind(&PeoplemonMenu::messageDone, this));
    }
    else {
        systems.hud().displayMessage(ppl.name() + " is not holding anything",
                                     std::bind(&PeoplemonMenu::messageDone, this));
    }
    state = MenuState::ShowingMessage;
    inputDriver.drive(nullptr);
    actionOpen = false;
}

void PeoplemonMenu::resetAction() {
    state = MenuState::Browsing;
    inputDriver.drive(&menu);
    actionOpen = false;
}

void PeoplemonMenu::connectButtons() {
    const unsigned int n = systems.player().team().size();
    int ml               = -1;
    int mr               = -1;

    for (unsigned int i = 0; i < n; i += 2) {
        if (buttons[i]) {
            ml = i;
            if (i >= 2) {
                menu.attachExisting(buttons[i].get(), buttons[i - 2].get(), bl::menu::Item::Bottom);
            }
            if (buttons[i + 1]) {
                menu.attachExisting(buttons[i + 1].get(), buttons[i].get(), bl::menu::Item::Right);
                if (i >= 2) {
                    menu.attachExisting(
                        buttons[i + 1].get(), buttons[i - 1].get(), bl::menu::Item::Bottom);
                    mr = i + 1;
                }
            }
        }
    }

    if (context != Context::BattleFaint) {
        if (mr >= 0) {
            menu.attachExisting(backBut.get(), buttons[mr].get(), bl::menu::Item::Bottom);
            menu.attachExisting(backBut.get(), buttons[ml].get(), bl::menu::Item::Bottom, false);
        }
        else {
            menu.attachExisting(backBut.get(), buttons[ml].get(), bl::menu::Item::Bottom);
        }
    }
}

void PeoplemonMenu::chosen() {
    if (data) data->chosen = mover1;
    systems.engine().popState();
}

void PeoplemonMenu::messageDone() {
    state = MenuState::Browsing;
    inputDriver.drive(&menu);
}

} // namespace state
} // namespace game
