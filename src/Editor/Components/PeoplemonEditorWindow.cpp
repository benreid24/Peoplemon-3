#include <Editor/Components/PeoplemonEditorWindow.hpp>

#include <Core/Peoplemon/Move.hpp>
#include <Core/Resources.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;
using namespace core::pplmn;
using core::file::PeoplemonDB;

namespace
{
std::string moveToStr(MoveId move) {
    std::stringstream ss;
    ss << "Id: " << static_cast<int>(move) << " | ";
    ss << Move::name(move);
    return ss.str();
}

std::string learnMoveToStr(const std::pair<unsigned int, MoveId>& lm) {
    std::stringstream ss;
    ss << "Lvl " << lm.first << " | " << Move::name(lm.second);
    return ss.str();
}

template<typename T>
T parseInput(const bl::gui::TextEntry::Ptr& input) {
    const long long val = std::atoll(input->getInput().c_str());
    return static_cast<T>(val);
}

void error(const std::string& e) {
    bl::dialog::tinyfd_messageBox("Error", e.c_str(), "ok", "error", 1);
}

bool warning(const std::string& e) {
    return 1 == bl::dialog::tinyfd_messageBox("Warning", e.c_str(), "yesno", "warning", 0);
}

} // namespace

PeoplemonEditorWindow::PeoplemonEditorWindow(PeoplemonDB& db, const OnChange& onChange)
: peoplemonDb(db)
, onChange(onChange)
, statEntry(StatBox::IV, false)
, evAwardEntry(StatBox::IV, false) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Editing Peoplemon");
    window->getSignal(Event::Closed)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onCancel, this));

    LinePacker::Ptr rowPack = LinePacker::create(LinePacker::Horizontal, 4.f);
    LinePacker::Ptr colPack = LinePacker::create(LinePacker::Vertical, 4.f);
    const auto onEdit       = std::bind(&PeoplemonEditorWindow::makeDirty, this);
    applyBut                = Button::create("Save");

    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onCancel, this));
    applyBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::onSave, this));

    idEntry = TextEntry::create();
    idEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    idEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    idEntry->setRequisition({60.f, 1.f});
    Button::Ptr imgReload = Button::create("Reload Images");
    imgReload->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PeoplemonEditorWindow::reloadImages, this));
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

    abilitySelect = ComboBox::create();
    abilitySelect->addOption("None");
    abilitySelect->addOption("BoardGameMaster");
    abilitySelect->addOption("Chillaxed");
    abilitySelect->addOption("Classy");
    abilitySelect->addOption("Goon");
    abilitySelect->addOption("QuickDraw");
    abilitySelect->addOption("AlwaysFriendly");
    abilitySelect->addOption("Sassy");
    abilitySelect->addOption("ImpulseBuy");
    abilitySelect->addOption("RunAway");
    abilitySelect->addOption("MrExtra");
    abilitySelect->addOption("BeefedUp");
    abilitySelect->addOption("Reserved");
    abilitySelect->addOption("DukeOfJokes");
    abilitySelect->addOption("Engaging");
    abilitySelect->addOption("SnackShare");
    abilitySelect->addOption("DerpDerp");
    abilitySelect->addOption("Klutz");
    abilitySelect->addOption("GenderBend");
    abilitySelect->addOption("Garbage");
    abilitySelect->addOption("BikeMechanic");
    abilitySelect->addOption("SidetrackTeach");
    abilitySelect->addOption("NoJokeTeach");
    abilitySelect->addOption("FieryTeach");
    abilitySelect->addOption("ExperiencedTeach");
    abilitySelect->addOption("NewTeach");
    abilitySelect->addOption("DozeOff");
    abilitySelect->addOption("DouseFlames");
    abilitySelect->addOption("Flirty");
    abilitySelect->addOption("UndyingFaith");
    abilitySelect->addOption("TooCool");
    abilitySelect->addOption("FakeStudy");
    abilitySelect->addOption("Alcoholic");
    abilitySelect->addOption("TotalBro");
    abilitySelect->addOption("TotalMom");
    abilitySelect->addOption("CantSwim");
    abilitySelect->addOption("AllNighter");
    abilitySelect->addOption("AilmentSaturated");
    abilitySelect->addOption("Adament");
    abilitySelect->addOption("AbsolutePitch");
    abilitySelect->addOption("GamemakerVirus");
    abilitySelect->addOption("Snapshot");
    abilitySelect->addOption("GetBaked");
    abilitySelect->setMaxHeight(300.f);
    xpGroupSelect = ComboBox::create();
    xpGroupSelect->addOption("Slow");
    xpGroupSelect->addOption("Standard");
    xpGroupSelect->addOption("Fast");
    xpGroupSelect->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    evolveLevelEntry = TextEntry::create();
    evolveLevelEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    evolveLevelEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    evolveLevelEntry->setRequisition({60.f, 1.f});
    evolveIdSelect = PeoplemonSelector::create(true);
    evolveIdSelect->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    xpRewardMultEntry = TextEntry::create();
    xpRewardMultEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    xpRewardMultEntry->setRequisition({30.f, 1.f});
    xpRewardMultEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    catchRateEntry = TextEntry::create();
    catchRateEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    catchRateEntry->setRequisition({30.f, 1.f});
    catchRateEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);

    thumbTxtr = playerTxtr = opTxtr =
        TextureManager::load(Peoplemon::thumbnailImage(Id::Unknown)).data;
    thumbImg = Image::create(*thumbTxtr);
    thumbImg->scaleToSize({100.f, 100.f});
    playerImg = Image::create(*playerTxtr);
    playerImg->scaleToSize({150.f, 150.f});
    opImg = Image::create(*opTxtr);
    opImg->scaleToSize({150.f, 150.f});

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
    row->pack(col);
    window->pack(row, true, false);

    row    = Box::create(rowPack);
    col    = Box::create(colPack);
    subRow = Box::create(rowPack);
    col->pack(Label::create("Base Stats:"));
    statEntry.pack(*col);
    row->pack(col, false, true);
    col = Box::create(colPack);
    col->pack(Label::create("EV Award"));
    evAwardEntry.pack(*col);
    row->pack(col, false, true);

    col = Box::create(colPack);
    subRow->pack(Label::create("Special Ability:"), false, true);
    subRow->pack(abilitySelect, true, true);
    col->pack(subRow, true, false);
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
    subRow = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    subRow->pack(cancelBut);
    col->pack(subRow, true, false);
    subRow->pack(applyBut, false, true);
    row->pack(col, false, true);

    col             = Box::create(colPack);
    subRow          = Box::create(rowPack);
    Box::Ptr subCol = Box::create(colPack);
    subCol->pack(Label::create("Thumbnail:"));
    subCol->pack(thumbImg);
    subRow->pack(subCol, false, true);
    subRow->pack(imgReload);
    col->pack(subRow, true, false);
    subRow = Box::create(rowPack);
    subCol = Box::create(colPack);
    subCol->pack(Label::create("Player:"));
    subCol->pack(playerImg);
    subRow->pack(subCol, false, true);
    subCol = Box::create(colPack);
    subCol->pack(Label::create("Opponent:"));
    subCol->pack(opImg);
    subRow->pack(subCol, false, true);
    col->pack(subRow, true, false);
    row->pack(col, false, true);
    window->pack(row, true, false);
}

void PeoplemonEditorWindow::open(const GUI::Ptr& parent, Id ppl) {
    openId            = ppl;
    doingNewPeoplemon = ppl == Id::Unknown;

    movePoolSelect->refresh();
    moveLearnSelect->refresh();
    evolveIdSelect->refresh();

    const std::pair<Type, Type> types =
        TypeUtil::getTypes(doingNewPeoplemon ? Type::None : Peoplemon::type(ppl));
    movePool.clear();
    learnMoves.clear();
    if (!doingNewPeoplemon) {
        const auto& mp = peoplemonDb.validMoves[ppl];
        movePool.reserve(mp.size());
        for (const MoveId move : mp) { movePool.emplace_back(move); }

        const auto& lm = peoplemonDb.learnedMoves[ppl];
        learnMoves.reserve(lm.size());
        for (const auto& mp : lm) { learnMoves.emplace_back(mp.first, mp.second); }
    }

    idEntry->setInput(doingNewPeoplemon ? "0" : std::to_string(static_cast<int>(ppl)));
    nameEntry->setInput(doingNewPeoplemon ? "" : Peoplemon::name(ppl));
    descEntry->setInput(doingNewPeoplemon ? "" : Peoplemon::description(ppl));
    type1Select->setCurrentType(types.first);
    type2Select->setCurrentType(types.second);
    statEntry.update(doingNewPeoplemon ? Stats{} : Peoplemon::baseStats(ppl));

    for (const MoveId move : movePool) { movePoolEntry->addOption(moveToStr(move)); }
    movePoolSelect->selectRandom();
    for (const auto& mp : learnMoves) { learnMoveEntry->addOption(learnMoveToStr(mp)); }
    moveLevelEntry->setInput("30");
    moveLearnSelect->selectRandom();

    abilitySelect->setSelectedOption(
        doingNewPeoplemon ? 0 : static_cast<int>(Peoplemon::specialAbility(ppl)));
    evolveLevelEntry->setInput(doingNewPeoplemon ? "0" :
                                                   std::to_string(Peoplemon::evolveLevel(ppl)));
    evolveIdSelect->setPeoplemon(doingNewPeoplemon ? Id::Unknown : Peoplemon::evolvesInto(ppl));
    xpGroupSelect->setSelectedOption(doingNewPeoplemon ? 1 : peoplemonDb.xpGroups[ppl]);
    xpRewardMultEntry->setInput(
        doingNewPeoplemon ? "1" : std::to_string(Peoplemon::xpYieldMultiplier(ppl)));
    evAwardEntry.update(doingNewPeoplemon ? Stats{} : Peoplemon::evAward(ppl));
    catchRateEntry->setInput(
        doingNewPeoplemon ? "48" : std::to_string(static_cast<int>(Peoplemon::catchRate(ppl))));

    reloadImages();

    dirty = false;
    applyBut->setColor(sf::Color(20, 240, 50), sf::Color::Black);
    parent->pack(window);
    window->setForceFocus(true);
}

void PeoplemonEditorWindow::reloadImages() {
    const Id ppl = parseInput<Id>(idEntry);
    thumbTxtr    = TextureManager::load(Peoplemon::thumbnailImage(ppl)).data;
    playerTxtr   = TextureManager::load(Peoplemon::playerImage(ppl)).data;
    opTxtr       = TextureManager::load(Peoplemon::opponentImage(ppl)).data;
    thumbImg->setImage(thumbTxtr, false);
    playerImg->setImage(playerTxtr, false);
    opImg->setImage(opTxtr, false);
}

void PeoplemonEditorWindow::makeDirty() {
    dirty = true;
    applyBut->setColor(sf::Color(230, 30, 30), sf::Color::Black);
}

void PeoplemonEditorWindow::onCancel() {
    if (dirty) {
        if (!warning("Discard unsaved changes?")) { return; }
    }
    close();
}

void PeoplemonEditorWindow::close() {
    window->setForceFocus(false);
    window->remove();
}

void PeoplemonEditorWindow::onMovePoolAdd() {
    const MoveId move = movePoolSelect->currentMove();
    if (move == MoveId::Unknown) {
        error("Please select a move to add");
        return;
    }
    for (const MoveId vm : movePool) {
        if (vm == move) {
            error("This move is already in the move pool");
            return;
        }
    }
    movePool.emplace_back(move);
    movePoolEntry->addOption(moveToStr(move));
    makeDirty();
}

void PeoplemonEditorWindow::onMovePoolDelete() {
    const auto io = movePoolEntry->getSelectedOption();
    if (!io.has_value()) {
        error("Please select a move to delete");
        return;
    }
    const unsigned int i = io.value();
    if (i >= movePool.size() || i >= movePoolEntry->optionCount()) {
        error("Somehow in invalid state. Move index is greater than move count");
        return;
    }
    movePool.erase(movePool.begin() + i);
    movePoolEntry->removeOption(i);
    makeDirty();
}

void PeoplemonEditorWindow::onLearnMoveAdd() {
    const unsigned int lvl = std::atoi(moveLevelEntry->getInput().c_str());
    const MoveId move      = moveLearnSelect->currentMove();

    if (lvl == 0 || lvl > 100) {
        error("Level is out of range [1, 100]");
        return;
    }
    if (move == MoveId::Unknown) {
        error("Please select a move to learn");
        return;
    }
    for (const auto& lp : learnMoves) {
        if (lp.first == lvl) {
            error("A move is already being learned at this level");
            return;
        }
    }
    learnMoves.emplace_back(lvl, move);
    learnMoveEntry->addOption(learnMoveToStr(learnMoves.back()));
    makeDirty();
}

void PeoplemonEditorWindow::onLearnMoveDelete() {
    const auto io = learnMoveEntry->getSelectedOption();
    if (!io.has_value()) {
        error("Please select a learned move to delete");
        return;
    }
    const unsigned int i = io.value();
    if (i >= learnMoves.size() || i >= learnMoveEntry->optionCount()) {
        error("In invalid state. Index is out of range");
        return;
    }
    learnMoves.erase(learnMoves.begin() + i);
    learnMoveEntry->removeOption(i);
    makeDirty();
}

void PeoplemonEditorWindow::onSave() {
    const Id id                = parseInput<Id>(idEntry);
    const Id eid               = evolveIdSelect->currentPeoplemon();
    const unsigned int el      = parseInput<unsigned int>(evolveLevelEntry);
    const unsigned int xpGroup = xpGroupSelect->getSelectedOption();
    const int xpYieldMult      = parseInput<int>(xpRewardMultEntry);
    const unsigned int cr      = parseInput<unsigned int>(catchRateEntry);
    const Type type            = type1Select->currentType() | type2Select->currentType();
    const SpecialAbility ab    = static_cast<SpecialAbility>(abilitySelect->getSelectedOption());

    if (id == Id::Unknown) {
        error("Invalid id");
        return;
    }
    if (openId != id && peoplemonDb.names.find(id) != peoplemonDb.names.end()) {
        if (!warning("Id already exists. Overwrite it?")) { return; }
    }

    if (nameEntry->getInput().empty()) {
        error("Please enter a name");
        return;
    }
    if (descEntry->getInput().empty()) {
        error("Please a description");
        return;
    }
    if (type == Type::None) {
        error("Please select at least one type");
        return;
    }
    if (statEntry.currentValue().sum() == 0) {
        if (!warning("Base stats are all 0. Continue anyways?")) { return; }
    }

    if (movePool.empty()) {
        if (!warning("Valid move pool is empty. Continue?")) { return; }
    }
    if (learnMoves.empty()) {
        if (!warning("No moves are learned by leveling. Continue?")) { return; }
    }

    if (eid != Id::Unknown) {
        if (el == 0 || el > 100) {
            error("Evolve level is out of range [1, 100]");
            return;
        }
    }

    if (xpGroup > 2) {
        error("Invalid level-up speed");
        return;
    }
    if (xpYieldMult < 0) {
        error("Invalid XP yield multiplier");
        return;
    }
    if (xpYieldMult == 0) {
        if (!warning("Rewards 0 XP on defeat. Continue?")) { return; }
    }
    if (evAwardEntry.currentValue().sum() == 0) {
        if (!warning("Rewards no EVs. Continue?")) { return; }
    }
    if (cr <= 0 || cr > 255) {
        error("Catch rate is out of range [1, 255]");
        return;
    }

    peoplemonDb.names[id]        = nameEntry->getInput();
    peoplemonDb.descriptions[id] = descEntry->getInput();
    peoplemonDb.types[id]        = type;
    peoplemonDb.abilities[id]    = ab;
    peoplemonDb.stats[id]        = statEntry.currentValue();

    peoplemonDb.validMoves[id] = {};
    auto& mp                   = peoplemonDb.validMoves[id];
    for (const MoveId move : movePool) { mp.emplace(move); }
    peoplemonDb.learnedMoves[id] = {};
    auto& lms                    = peoplemonDb.learnedMoves[id];
    for (const auto& lm : learnMoves) { lms.emplace(lm.first, lm.second); }

    peoplemonDb.evolveLevels[id] = el;
    peoplemonDb.evolveIds[id]    = eid;
    peoplemonDb.evAwards[id]     = evAwardEntry.currentValue();
    peoplemonDb.xpGroups[id]     = xpGroup;
    peoplemonDb.xpMults[id]      = xpYieldMult;
    peoplemonDb.catchRates[id]   = cr;

    onChange();
    close();
}

} // namespace component
} // namespace editor
