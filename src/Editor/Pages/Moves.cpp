#include <Editor/Pages/Moves.hpp>

#include <Core/Peoplemon/Move.hpp>

namespace editor
{
namespace page
{
namespace
{
std::vector<core::pplmn::MoveId> getSorted(const core::file::MoveDB& db) {
    std::vector<core::pplmn::MoveId> ids;
    ids.reserve(db.names.size());
    for (const auto& p : db.names) { ids.emplace_back(p.first); }
    std::sort(ids.begin(), ids.end());
    return ids;
}
} // namespace

using namespace bl::gui;

Moves::Moves(core::system::Systems& s)
: Page(s)
, moveWindow(moveDb, std::bind(&Moves::onChange, this)) {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 8.f));

    Box::Ptr row    = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    Button::Ptr but = Button::create("New Move");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Moves::newMove, this));
    row->pack(but, false, true);
    saveBut = Button::create("Save");
    saveBut->setColor(sf::Color::Green, sf::Color::Black);
    saveBut->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Moves::save, this));
    row->pack(saveBut, false, true);
    but = Button::create("Reset");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Moves::reset, this));
    but->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(but, false, true);
    content->pack(row, true, false);

    rowArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4.f));
    content->pack(rowArea, true, true);

    if (!moveDb.load()) BL_LOG_ERROR << "Failed to load move database";
    core::pplmn::Move::setDataSource(moveDb);
    sync();
}

void Moves::sync() {
    rowArea->clearChildren(true);
    const std::vector<core::pplmn::MoveId> allIds = getSorted(moveDb);

    int i                   = 0;
    LinePacker::Ptr rowPack = LinePacker::create(LinePacker::Horizontal, 4.f, LinePacker::Uniform);
    for (core::pplmn::MoveId id : allIds) {
        Box::Ptr row = Box::create(rowPack);
        row->setColor(i % 2 == 0 ? sf::Color(185, 185, 185) : sf::Color(70, 70, 70),
                      sf::Color(20, 85, 230));
        Label::Ptr lbl = Label::create("Id: " + std::to_string(static_cast<int>(id)));
        lbl->setColor(sf::Color(30, 85, 255), sf::Color::Transparent);
        row->pack(lbl, false, true);
        lbl = Label::create(core::pplmn::Move::name(id));
        lbl->setColor(sf::Color(255, 60, 90), sf::Color::Transparent);
        row->pack(lbl, false, true);
        Button::Ptr but = Button::create("Edit");
        but->setColor(sf::Color(230, 230, 30), sf::Color::Black);
        but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Moves::editMove, this, id));
        row->pack(but, false, true);
        but = Button::create("Delete");
        but->setColor(sf::Color(230, 30, 30), sf::Color::Black);
        but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Moves::deleteMove, this, id));
        row->pack(but, false, true);
        rowArea->pack(row, true, false);
        ++i;
    }
}

void Moves::makeDirty() { saveBut->setColor(sf::Color::Yellow, sf::Color::Black); }

void Moves::save() {
    if (!moveDb.save()) { BL_LOG_ERROR << "Failed to save move database"; }
    else {
        saveBut->setColor(sf::Color::Green, sf::Color::Black);
    }
}

void Moves::newMove() { moveWindow.open(parent, core::pplmn::MoveId::Unknown); }

void Moves::editMove(core::pplmn::MoveId move) { moveWindow.open(parent, move); }

void Moves::reset() {
    if (!moveDb.load()) { BL_LOG_ERROR << "Failed to load move database"; }
    else {
        sync();
        saveBut->setColor(sf::Color::Green, sf::Color::Black);
    }
}

void Moves::deleteMove(core::pplmn::MoveId move) {
    makeDirty();

    moveDb.names.erase(move);
    moveDb.descriptions.erase(move);
    moveDb.types.erase(move);
    moveDb.damages.erase(move);
    moveDb.accuracies.erase(move);
    moveDb.priorities.erase(move);
    moveDb.pps.erase(move);
    moveDb.contactors.erase(move);
    moveDb.specials.erase(move);
    moveDb.effects.erase(move);
    moveDb.effectChances.erase(move);
    moveDb.effectIntensities.erase(move);
    moveDb.effectSelves.erase(move);

    sync();
}

void Moves::onChange() {
    sync();
    makeDirty();
}

void Moves::update(float) {}

} // namespace page
} // namespace editor
