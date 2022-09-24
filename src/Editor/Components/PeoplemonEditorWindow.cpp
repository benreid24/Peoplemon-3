#include <Editor/Components/PeoplemonEditorWindow.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;
using namespace core::pplmn;
using core::file::PeoplemonDB;

PeoplemonEditorWindow::PeoplemonEditorWindow(PeoplemonDB& db, const OnChange& onChange)
: peoplemonDb(db)
, onChange(onChange)
, statEntry(StatBox::IV)
, evAwardEntry(StatBox::IV) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Editing Peoplemon");
    window->getSignal(Event::Closed)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onCancel, this));

    LinePacker::Ptr rowPack = LinePacker::create(LinePacker::Horizontal, 4.f);
    LinePacker::Ptr colPack = LinePacker::create(LinePacker::Vertical, 4.f);
    const auto onEdit       = std::bind(&PeoplemonEditorWindow::makeDirty, this);
    applyBut                = Button::create("Save");

    idEntry = TextEntry::create();
    idEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    idEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    idEntry->setRequisition({60.f, 1.f});
    nameEntry = TextEntry::create();
    nameEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    nameEntry->setRequisition({160.f, 1.f});
    descEntry = TextEntry::create();
    descEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    descEntry->setRequisition({400.f, 1.f});
    type1Select = TypeSelector::create();
    type2Select = TypeSelector::create();

    movePoolEntry = SelectBox::create();
    movePoolEntry->setRequisition({200.f, 1.f});
    movePoolEntry->setMaxSize({250.f, 300.f});
    movePoolSelect             = MoveSelector::create(false);
    Button::Ptr movePoolAddBut = Button::create("Add");
    movePoolAddBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onMovePoolAdd, this));
    Button::Ptr movePoolRmBut = Button::create("Remove");
    movePoolRmBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onMovePoolDelete, this));
    movePoolRmBut->setColor(sf::Color::Red, sf::Color::Black);

    learnMoveEntry = SelectBox::create();
    learnMoveEntry->setRequisition({200.f, 1.f});
    learnMoveEntry->setMaxSize({250.f, 300.f});
    moveLearnSelect = MoveSelector::create(false);
    moveLevelEntry  = TextEntry::create();
    moveLevelEntry->setRequisition({45.f, 1.f});
    Button::Ptr moveLearnAddBut = Button::create("Add");
    moveLearnAddBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onLearnMoveAdd, this));
    Button::Ptr moveLearnRmBut = Button::create("Remove");
    moveLearnRmBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onLearnMoveDelete, this));
    moveLearnRmBut->setColor(sf::Color::Red, sf::Color::Black);

    xpGroupSelect = ComboBox::create();
    xpGroupSelect->addOption("Slow");
    xpGroupSelect->addOption("Standard");
    xpGroupSelect->addOption("Fast");
    xpGroupSelect->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    evolveLevelEntry = TextEntry::create();
    evolveLevelEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    evolveLevelEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    evolveLevelEntry->setRequisition({60.f, 1.f});
    evolveIdSelect = PeoplemonSelector::create();
    evolveIdSelect->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    xpRewardMultEntry = TextEntry::create();
    xpRewardMultEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    xpRewardMultEntry->setRequisition({30.f, 1.f});
    xpRewardMultEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    catchRateEntry = TextEntry::create();
    catchRateEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    catchRateEntry->setRequisition({30.f, 1.f});
    catchRateEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);

    Box::Ptr row    = Box::create(rowPack);
    Box::Ptr col    = Box::create(colPack);
    Box::Ptr subRow = Box::create(rowPack);

    row->pack(Label::create("ID:"), false, true);
    row->pack(idEntry, false, true);
    row->pack(Label::create("Name:"), false, true);
    row->pack(nameEntry, false, true);
    row->pack(Label::create("Type:"), false, true);
    row->pack(type1Select, false, true);
    row->pack(Label::create("Secondary Type:"), false, true);
    row->pack(type2Select, false, true);
    window->pack(row, true, false);

    row = Box::create(rowPack);
    row->pack(Label::create("Description:"), false, true);
    row->pack(descEntry, true, true);
    window->pack(row, true, false);

    row = Box::create(rowPack);
    col->pack(Label::create("Base Stats:"));
    statEntry.pack(*col);
    row->pack(col, false, true);
    col = Box::create(colPack);
    col->pack(Label::create("Move Pool"));
    col->pack(movePoolEntry, true, true);
    row->pack(col, false, true);
    col = Box::create(colPack);
    col->pack(movePoolSelect, true, false);
    col->pack(movePoolAddBut);
    col->pack(movePoolRmBut);
    row->pack(col);
    col = Box::create(colPack);
    col->pack(Label::create("Learned Moves"));
    col->pack(learnMoveEntry, true, true);
    row->pack(col, false, true);
    col = Box::create(colPack);
    subRow->pack(Label::create("Lvl:"), false, true);
    subRow->pack(moveLevelEntry, true, true);
    col->pack(subRow, true, false);
    col->pack(moveLearnSelect, true, false);
    col->pack(moveLearnAddBut);
    col->pack(moveLearnRmBut);
    row->pack(col, false, true);
    window->pack(row, true, false);

    row    = Box::create(rowPack);
    col    = Box::create(colPack);
    subRow = Box::create(rowPack);
    subRow->pack(Label::create("Level-up Speed:"), false, true);
    subRow->pack(xpGroupSelect, true, true);
    col->pack(subRow, true, false);
    subRow = Box::create(rowPack);
    subRow->pack(Label::create("Evolve Level:"), false, true);
    subRow->pack(evolveLevelEntry, true, true);
    col->pack(subRow, true, false);
    subRow = Box::create(rowPack);
    subRow->pack(Label::create("Evolve Into:"), false, true);
    subRow->pack(evolveIdSelect, true, true);
    col->pack(subRow, true, false);
    subRow = Box::create(rowPack);
    subRow->pack(Label::create("XP-Yield Multiplier:"), false, true);
    subRow->pack(xpRewardMultEntry, true, true);
    col->pack(subRow, true, false);
    subRow = Box::create(rowPack);
    subRow->pack(Label::create("Wild Catch Rate:"), false, true);
    subRow->pack(catchRateEntry, true, true);
    col->pack(subRow, true, false);
    row->pack(col, false, true);
    col = Box::create(colPack);
    col->pack(Label::create("EV Award"));
    evAwardEntry.pack(*col);
    row->pack(col, false, true);
    window->pack(row, true, false);

    row                   = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onCancel, this));
    row->pack(cancelBut, false, true);
    applyBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onSave, this));
    row->pack(applyBut, false, true);
    window->pack(row, true, false);
}

void PeoplemonEditorWindow::open(const GUI::Ptr& parent, Id ppl) {
    openId            = ppl;
    doingNewPeoplemon = ppl == Id::Unknown;

    movePoolSelect->refresh();
    moveLearnSelect->refresh();
    evolveIdSelect->refresh();

    // TODO - set values

    dirty = false;
    applyBut->setColor(sf::Color(20, 240, 50), sf::Color::Black);
    parent->pack(window);
    window->setForceFocus(true);
}

// TODO - other methods

} // namespace component
} // namespace editor
