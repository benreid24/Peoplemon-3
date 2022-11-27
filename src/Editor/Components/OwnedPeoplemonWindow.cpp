#include <Editor/Components/OwnedPeoplemonWindow.hpp>

#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

OwnedPeoplemonWindow::OwnedPeoplemonWindow(const NotifyCB& fcb, const NotifyCB& ccb)
: onFinish(fcb)
, onCancel(ccb)
, evBox(StatBox::EV)
, ivBox(StatBox::IV) {
    window =
        Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Owned Peoplemon Editor");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) {
        hide();
        onCancel();
    });

    idSelect = PeoplemonSelector::create();
    idSelect->setMaxHeight(400.f);
    idSelect->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        const auto ppl = getValue();
        moveSelector->notifyPeoplemon(ppl.id(), ppl.currentLevel());
    });
    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Species:"), false, true);
    row->pack(idSelect, false, true);
    window->pack(row);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Nickname:"), false, true);
    nameEntry = TextEntry::create();
    nameEntry->setRequisition({80.f, 1.f});
    row->pack(nameEntry, false, true);
    window->pack(row);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Level:"), false, true);
    levelEntry = TextEntry::create();
    levelEntry->setInput("50");
    levelEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    levelEntry->setRequisition({60.f, 1.f});
    levelEntry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        const unsigned int level = std::atoi(levelEntry->getInput().c_str());
        evBox.notifyLevel(level);
        moveSelector->notifyPeoplemon(getValue().id(), level);
    });
    row->pack(levelEntry, false, true);
    window->pack(row);

    Label::Ptr l = Label::create("Moves:");
    l->setHorizontalAlignment(RenderSettings::Alignment::Left);
    window->pack(l, true, false);
    row     = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    moveBox = SelectBox::create();
    moveBox->setMaxSize({1200.f, 90.f});
    moveBox->setRequisition({250.f, 108.f});
    row->pack(moveBox, true, true);
    Box::Ptr box    = Box::create(LinePacker::create(LinePacker::Vertical, 8.f));
    Box::Ptr subRow = Box::create(LinePacker::create());
    moveSelector    = MoveSelector::create(true);
    subRow->pack(moveSelector, false, true);
    box->pack(subRow, true, false);
    subRow = Box::create(LinePacker::create());
    Button::Ptr but = Button::create("Add");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (moves.size() < 4) {
            if (std::find(moves.begin(), moves.end(), moveSelector->currentMove()) == moves.end()) {
                moves.push_back(moveSelector->currentMove());
                moveBox->addOption(core::pplmn::Move::name(moveSelector->currentMove()));
            }
        }
    });
    subRow->pack(but, false, true);
    but = Button::create("Random");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        moveSelector->selectRandom();
    });
    subRow->pack(but, false, false);
    but = Button::create("Remove");
    but->setColor(sf::Color::Red, sf::Color::Black);
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        const auto sel = moveBox->getSelectedOption();
        if (moves.size() > 1 && sel.has_value()) {
            moves.erase(moves.begin() + sel.value());
            moveBox->removeOption(sel.value());
        }
    });
    subRow->pack(but, false, true);
    box->pack(subRow, true, false);
    row->pack(box);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    box = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    box->pack(Label::create("EVs"), true, false);
    evBox.pack(*box);
    row->pack(box, true, true);
    box = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    box->pack(Label::create("IVs"), true, false);
    ivBox.pack(*box);
    row->pack(box, true, true);
    window->pack(row);

    row          = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    itemSelector = ItemSelector::create();
    but          = Button::create("Clear");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        itemSelector->setSelectedOption(-1);
    });
    row->pack(Label::create("Hold item:"), false, true);
    row->pack(itemSelector, false, true);
    row->pack(but, false, true);
    window->pack(row);

    but = Button::create("Save");
    but->setColor(sf::Color::Green, sf::Color::Black);
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (validate()) {
            hide();
            onFinish();
        }
    });
    window->pack(but);
}

void OwnedPeoplemonWindow::show(const GUI::Ptr& p, const core::pplmn::OwnedPeoplemon& value) {
    parent = p;

    if (core::pplmn::Peoplemon::name(value.id()) == value.name()) { nameEntry->setInput(""); }
    else {
        nameEntry->setInput(value.name());
    }

    levelEntry->setInput(std::to_string(value.currentLevel()));
    idSelect->setPeoplemon(value.id());

    evBox.update(value.currentEVs());
    ivBox.update(value.currentIVs());

    itemSelector->setItem(value.holdItem());
    itemSelector->refresh();

    moveBox->clearOptions();
    moves.clear();
    moves.reserve(4);
    for (unsigned int i = 0; i < 4; ++i) {
        if (value.moves[i].id != core::pplmn::MoveId::Unknown) {
            moves.push_back(value.moves[i].id);
            moveBox->addOption(core::pplmn::Move::name(value.moves[i].id));
        }
    }
    moveSelector->notifyPeoplemon(value.id(), value.currentLevel());

    parent->pack(window);
    window->setForceFocus(true);
}

void OwnedPeoplemonWindow::hide() {
    window->setForceFocus(false);
    window->remove();
}

core::pplmn::OwnedPeoplemon OwnedPeoplemonWindow::getValue() const {
    core::pplmn::OwnedPeoplemon val(idSelect->currentPeoplemon(),
                                    std::atoi(levelEntry->getInput().c_str()));
    val.ivs  = ivBox.currentValue();
    val.evs  = evBox.currentValue();
    val.item = itemSelector->currentItem();
    val.hp   = val.currentStats().hp;
    for (unsigned int i = 0; i < 4; ++i) {
        if (i < moves.size()) { val.moves[i] = core::pplmn::OwnedMove(moves[i]); }
        else {
            val.moves[i].id = core::pplmn::MoveId::Unknown;
        }
    }
    return val;
}

void OwnedPeoplemonWindow::syncMoves() {}

bool OwnedPeoplemonWindow::validate() const {
    const unsigned int level = std::atoi(levelEntry->getInput().c_str());
    if (level > 100) {
        bl::dialog::tinyfd_messageBox(
            "Warning", "Level cannot be greater than 100", "ok", "warning", 1);
        return false;
    }

    const core::pplmn::Id id = idSelect->currentPeoplemon();
    if (id == core::pplmn::Id::Unknown) {
        bl::dialog::tinyfd_messageBox("Warning", "Please select a species", "ok", "warning", 1);
        return false;
    }

    if (moves.empty()) {
        bl::dialog::tinyfd_messageBox(
            "Warning", "Please add at least one move", "ok", "warning", 1);
        return false;
    }

    return true;
}

} // namespace component
} // namespace editor
