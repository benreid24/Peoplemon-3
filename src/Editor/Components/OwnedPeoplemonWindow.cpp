#include <Editor/Components/OwnedPeoplemonWindow.hpp>

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

    idSelect = ComboBox::create();
    idSelect->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event& e, Element*) {
        const auto it = idMap.find(static_cast<int>(e.inputValue()));
        if (it != idMap.end() && it->second != core::pplmn::Id::Unknown) { syncMoves(it->second); }
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
    row->pack(levelEntry, false, true);
    window->pack(row);

    row          = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    Box::Ptr box = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    box->pack(Label::create("EVs"), true, false);
    evBox.pack(*box);
    row->pack(box, true, true);
    box = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    box->pack(Label::create("IVs"), true, false);
    ivBox.pack(*box);
    row->pack(box, true, true);
    window->pack(row);

    row             = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    itemSelector    = ItemSelector::create();
    Button::Ptr but = Button::create("Clear");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        itemSelector->setSelectedOption(-1);
    });
    row->pack(Label::create("Hold item:"), false, true);
    row->pack(itemSelector, false, true);
    row->pack(but, false, true);
    window->pack(row);

    but = Button::create("Save");
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

    idSelect->clearOptions();
    const std::vector<core::pplmn::Id>& validIds = core::pplmn::Peoplemon::validIds();
    int toSelect                                 = -1;
    for (unsigned int i = 0; i < validIds.size(); ++i) {
        idSelect->addOption(std::to_string(static_cast<unsigned int>(validIds[i])) + ": " +
                            core::pplmn::Peoplemon::name(validIds[i]));
        idMap[i] = validIds[i];
        if (validIds[i] == value.id()) toSelect = i;
    }
    idMap[-1] = core::pplmn::Id::Unknown;
    idSelect->setSelectedOption(toSelect, false);

    evBox.update(value.currentEVs());
    ivBox.update(value.currentIVs());
    itemSelector->setItem(value.holdItem());

    parent->pack(window);
    window->setForceFocus(true);
}

void OwnedPeoplemonWindow::hide() {
    window->setForceFocus(false);
    window->remove();
}

core::pplmn::OwnedPeoplemon OwnedPeoplemonWindow::getValue() const {
    const auto it = idMap.find(idSelect->getSelectedOption());
    core::pplmn::OwnedPeoplemon val(it->second, std::atoi(levelEntry->getInput().c_str()));
    val.ivs  = ivBox.currentValue();
    val.evs  = evBox.currentValue();
    val.item = itemSelector->currentItem();
    val.hp   = val.currentStats().hp;
    // TODO - moves
    return val;
}

void OwnedPeoplemonWindow::syncMoves(core::pplmn::Id ppl) {
    // TODO
}

bool OwnedPeoplemonWindow::validate() const {
    // TODO
    return false;
}

} // namespace component
} // namespace editor
