#include <Editor/Components/MoveEditorWindow.hpp>

#include <BLIB/Interfaces.hpp>
#include <Core/Files/MoveDB.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/MoveEffect.hpp>

namespace editor
{
namespace component
{
namespace
{
const char* moveEffectNames[]         = {"None",
                                 "Heal",
                                 "Poison",
                                 "Burn",
                                 "Paralyze",
                                 "Freeze",
                                 "Confuse",
                                 "LeechSeed",
                                 "Flinch",
                                 "Trap",
                                 "Sleep",
                                 "Protection",
                                 "Safegaurd",
                                 "Substitute",
                                 "HealBell",
                                 "CritUp",
                                 "AtkUp",
                                 "DefUp",
                                 "AccUp",
                                 "EvdUp",
                                 "SpdUp",
                                 "SpAtkUp",
                                 "SpDefUp",
                                 "CritDown",
                                 "AtkDown",
                                 "DefDown",
                                 "AccDown",
                                 "EvdDown",
                                 "SpdDown",
                                 "SpAtkDown",
                                 "SpDefDown",
                                 "Recoil",
                                 "Charge",
                                 "Suicide",
                                 "Counter",
                                 "MirrorCoat",
                                 "OnlySleeping",
                                 "Peanut",
                                 "SetBall",
                                 "WakeBoth",
                                 "Absorb",
                                 "Encore",
                                 "RandomMove",
                                 "BatonPass",
                                 "DieIn3Turns",
                                 "CritEvdUp",
                                 "BumpBall",
                                 "SpikeBall",
                                 "DeathSwap",
                                 "Gamble",
                                 "StayAlive",
                                 "MaxAtkMinAcc",
                                 "FrustConfuse",
                                 "Spikes",
                                 "DoubleFamily",
                                 "EnemyPPDown",
                                 "HealNext",
                                 "Roar",
                                 "FailOnMove64",
                                 "SleepHeal",
                                 "SpdAtkUp",
                                 "StealStats",
                                 "BlockBall",
                                 "SwipeBall",
                                 "DamageThenSwitch",
                                 "RoarCancelBallSpikes"};
constexpr std::size_t moveEffectCount = sizeof(moveEffectNames) / sizeof(moveEffectNames[0]);

int effectToIndex(core::pplmn::MoveEffect ef) {
    const unsigned int i = static_cast<unsigned int>(ef);
    return i < moveEffectCount ? i : 0;
}

template<typename T>
T parseInput(const bl::gui::TextEntry::Ptr& input) {
    const long long val = std::atoll(input->getInput().c_str());
    return static_cast<T>(val);
}

} // namespace

using namespace bl::gui;

MoveEditorWindow::MoveEditorWindow(core::file::MoveDB& moveDb, const OnChange& onChange)
: moveDb(moveDb)
, onChange(onChange)
, doingNewMove(false)
, dirty(false) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Editing Move");
    window->getSignal(Event::Closed).willAlwaysCall(std::bind(&MoveEditorWindow::onCancel, this));

    LinePacker::Ptr rowPack = LinePacker::create(LinePacker::Horizontal, 4.f);
    const auto onEdit       = std::bind(MoveEditorWindow::makeDirty, this);
    applyBut                = Button::create("Save");

    Box::Ptr row = Box::create(rowPack);
    idEntry      = TextEntry::create();
    idEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    idEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    idEntry->setRequisition({60.f, 1.f});
    nameEntry = TextEntry::create();
    nameEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    nameEntry->setRequisition({160.f, 1.f});
    row->pack(Label::create("Id:"), false, true);
    row->pack(idEntry, false, true);
    row->pack(Label::create("Name:"), false, true);
    row->pack(nameEntry, false, true);
    window->pack(row, true, false);

    row       = Box::create(rowPack);
    descEntry = TextEntry::create();
    descEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    descEntry->setRequisition({400.f, 1.f});
    row->pack(Label::create("Description:"), false, true);
    row->pack(descEntry, false, true);
    window->pack(row, true, false);

    row        = Box::create(rowPack);
    typeSelect = TypeSelector::create();
    typeSelect->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    row->pack(Label::create("Type:"), false, true);
    row->pack(typeSelect, false, true);
    priorityEntry = TextEntry::create();
    priorityEntry->setMode(TextEntry::Mode::Integer);
    priorityEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    priorityEntry->setRequisition({60.f, 1.f});
    row->pack(Label::create("Priority (-7 - 5):"));
    row->pack(priorityEntry, false, true);
    contactCheck = CheckButton::create("Makes contact");
    contactCheck->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    row->pack(contactCheck, false, true);
    specialCheck = CheckButton::create("Special");
    specialCheck->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    row->pack(specialCheck, false, true);
    window->pack(row, true, false);

    row      = Box::create(rowPack);
    dmgEntry = TextEntry::create();
    dmgEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    dmgEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    dmgEntry->setRequisition({60.f, 1.f});
    row->pack(Label::create("Power:"), false, true);
    row->pack(dmgEntry, false, true);
    accEntry = TextEntry::create();
    accEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    accEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    accEntry->setRequisition({60.f, 1.f});
    row->pack(Label::create("Accuracy:"), false, true);
    row->pack(accEntry, false, true);
    ppEntry = TextEntry::create();
    ppEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    ppEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    ppEntry->setRequisition({60.f, 1.f});
    row->pack(Label::create("PP:"), false, true);
    row->pack(ppEntry, false, true);
    window->pack(row, true, false);

    row          = Box::create(rowPack);
    effectSelect = ComboBox::create();
    effectSelect->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    effectSelect->setMaxHeight(300.f);
    for (std::size_t i = 0; i < moveEffectCount; ++i) {
        effectSelect->addOption(moveEffectNames[i]);
    }
    effectSelect->setSelectedOption(0);
    row->pack(Label::create("Effect"), false, true);
    row->pack(effectSelect, false, true);
    effectSelfCheck = CheckButton::create("Effects user");
    effectSelfCheck->getSignal(Event::ValueChanged).willAlwaysCall(onEdit);
    row->pack(effectSelfCheck, false, true);
    window->pack(row, true, false);

    row               = Box::create(rowPack);
    effectChanceEntry = TextEntry::create();
    effectChanceEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    effectChanceEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    effectChanceEntry->setRequisition({60.f, 1.f});
    row->pack(Label::create("Effect chance (0-100):"), false, true);
    row->pack(effectChanceEntry, false, true);
    effectIntenseEntry = TextEntry::create();
    effectIntenseEntry->setMode(TextEntry::Mode::Integer);
    effectIntenseEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    effectIntenseEntry->setRequisition({60.f, 1.f});
    row->pack(Label::create("Effect intensity:"), false, true);
    row->pack(effectIntenseEntry, false, true);
    window->pack(row, true, false);

    row                   = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&MoveEditorWindow::onCancel, this));
    row->pack(cancelBut, false, true);
    applyBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&MoveEditorWindow::onSave, this));
    row->pack(applyBut, false, true);
    window->pack(row, true, false);
}

void MoveEditorWindow::open(const bl::gui::GUI::Ptr& parent, core::pplmn::MoveId move) {
    using Move = core::pplmn::Move;

    openId       = move;
    doingNewMove = move == core::pplmn::MoveId::Unknown;

    idEntry->setInput(doingNewMove ? "" : std::to_string(static_cast<int>(move)));
    nameEntry->setInput(doingNewMove ? "" : Move::name(move));
    descEntry->setInput(doingNewMove ? "" : Move::description(move));
    typeSelect->setCurrentType(doingNewMove ? core::pplmn::Type::Normal : Move::type(move));
    dmgEntry->setInput(doingNewMove ? "100" : std::to_string(Move::damage(move)));
    accEntry->setInput(doingNewMove ? "100" : std::to_string(Move::accuracy(move)));
    priorityEntry->setInput(doingNewMove ? "0" : std::to_string(Move::priority(move)));
    ppEntry->setInput(doingNewMove ? "15" : std::to_string(Move::pp(move)));
    contactCheck->setValue(doingNewMove ? true : Move::makesContact(move));
    specialCheck->setValue(doingNewMove ? false : Move::isSpecial(move));
    effectSelect->setSelectedOption(doingNewMove ? 0 : effectToIndex(Move::effect(move)));
    effectSelfCheck->setValue(doingNewMove ? false : Move::affectsUser(move));
    effectChanceEntry->setInput(doingNewMove ? "0" : std::to_string(Move::effectChance(move)));
    effectIntenseEntry->setInput(doingNewMove ? "0" : std::to_string(Move::effectIntensity(move)));

    dirty = false;
    applyBut->setColor(sf::Color(20, 240, 50), sf::Color::Black);
    parent->pack(window);
    window->setForceFocus(true);
}

void MoveEditorWindow::makeDirty() {
    dirty = true;
    applyBut->setColor(sf::Color(230, 30, 30), sf::Color::Black);
}

void MoveEditorWindow::onSave() {
    using namespace core::pplmn;
    const auto error = [](const std::string& e) {
        bl::dialog::tinyfd_messageBox("Error", e.c_str(), "ok", "error", 1);
    };

    const MoveId id         = parseInput<MoveId>(idEntry);
    const int dmg           = parseInput<int>(dmgEntry);
    const int acc           = parseInput<int>(accEntry);
    const int pri           = parseInput<int>(priorityEntry);
    const int pp            = parseInput<int>(ppEntry);
    const MoveEffect effect = static_cast<MoveEffect>(effectSelect->getSelectedOption());
    const int effectChance  = parseInput<int>(effectChanceEntry);
    const int effectIntense = parseInput<int>(effectIntenseEntry);

    if (openId != id && moveDb.names.find(id) != moveDb.names.end()) {
        if (1 != bl::dialog::tinyfd_messageBox(
                     "Warning", "Id already exists. Overwrite it?", "yesno", "warning", 0)) {
            return;
        }
    }
    if (id == MoveId::Unknown) {
        error("Invalid id");
        return;
    }
    if (nameEntry->getInput().empty()) {
        error("Please enter a name");
        return;
    }
    if (descEntry->getInput().empty()) {
        error("Please enter a description");
        return;
    }
    if (typeSelect->currentType() == Type::None) {
        error("Please select a type");
        return;
    }
    if (effectSelect->getSelectedOption() < 0) {
        error("Please select an effect or None");
        return;
    }
    if (pri < -7 || pri > 5) {
        error("Priority out of range");
        return;
    }
    if (pp <= 0) {
        error("Invalid PP");
        return;
    }

    moveDb.names[id]             = nameEntry->getInput();
    moveDb.descriptions[id]      = descEntry->getInput();
    moveDb.types[id]             = typeSelect->currentType();
    moveDb.damages[id]           = dmg;
    moveDb.accuracies[id]        = acc;
    moveDb.priorities[id]        = pri;
    moveDb.pps[id]               = pp;
    moveDb.contactors[id]        = contactCheck->getValue();
    moveDb.specials[id]          = specialCheck->getValue();
    moveDb.effects[id]           = effect;
    moveDb.effectChances[id]     = effectChance;
    moveDb.effectIntensities[id] = effectIntense;
    moveDb.effectSelves[id]      = effectSelfCheck->getValue();

    onChange();
    close();
}

void MoveEditorWindow::onCancel() {
    if (dirty) {
        if (1 != bl::dialog::tinyfd_messageBox(
                     "Warning", "Discard unsaved changes?", "yesno", "warning", 0)) {
            return;
        }
    }
    close();
}

void MoveEditorWindow::close() {
    window->setForceFocus(false);
    window->remove();
}

} // namespace component
} // namespace editor
