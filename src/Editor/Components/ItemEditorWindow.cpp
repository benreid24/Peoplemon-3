#include <Editor/Components/ItemEditorWindow.hpp>

namespace editor
{
namespace component
{
namespace
{
template<typename T>
T parseInput(const bl::gui::TextEntry::Ptr& input) {
    const long long val = std::atoll(input->getInput().c_str());
    return static_cast<T>(val);
}
} // namespace
using namespace bl::gui;

ItemEditorWindow::ItemEditorWindow(core::file::ItemDB& db, const OnChange& onChange)
: itemDb(db)
, onChange(onChange) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Editing Item");
    window->getSignal(Event::Closed).willAlwaysCall(std::bind(&ItemEditorWindow::onCancel, this));

    LinePacker::Ptr rowPack = LinePacker::create(LinePacker::Horizontal, 4.f);
    const auto onEdit       = std::bind(&ItemEditorWindow::makeDirty, this);
    applyBut                = Button::create("Save");

    window->pack(Label::create("ID ranges:\n\t[1, 100] - Regular Items\n\t[101, 200] - Key "
                               "Items\n\t [201, inf] - TM's (MoveId = ItemId - 200)"));

    Box::Ptr row = Box::create(rowPack);
    idEntry      = TextEntry::create();
    idEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    idEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    idEntry->setRequisition({60.f, 1.f});
    nameEntry = TextEntry::create();
    nameEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    nameEntry->setRequisition({160.f, 1.f});
    valueEntry = TextEntry::create();
    valueEntry->setMode(TextEntry::Mode::Unsigned | TextEntry::Mode::Integer);
    valueEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    valueEntry->setRequisition({60.f, 1.f});
    row->pack(Label::create("Id:"), false, true);
    row->pack(idEntry, false, true);
    row->pack(Label::create("Name:"), false, true);
    row->pack(nameEntry, false, true);
    row->pack(Label::create("Value:"), false, true);
    row->pack(valueEntry, false, true);
    window->pack(row, true, false);

    row       = Box::create(rowPack);
    descEntry = TextEntry::create();
    descEntry->getSignal(Event::TextEntered).willAlwaysCall(onEdit);
    descEntry->setRequisition({400.f, 1.f});
    row->pack(Label::create("Description:"), false, true);
    row->pack(descEntry, false, true);
    window->pack(row, true, false);

    row                   = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&ItemEditorWindow::onCancel, this));
    row->pack(cancelBut, false, true);
    applyBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&ItemEditorWindow::onSave, this));
    row->pack(applyBut, false, true);
    window->pack(row, true, false);
}

void ItemEditorWindow::open(bl::gui::GUI* parent, core::item::Id item) {
    using Item = core::item::Item;

    openId       = item;
    doingNewItem = item == core::item::Id::Unknown;

    idEntry->setInput(doingNewItem ? "" : std::to_string(static_cast<int>(item)));
    nameEntry->setInput(doingNewItem ? "" : Item::getName(item));
    descEntry->setInput(doingNewItem ? "" : Item::getDescription(item));
    valueEntry->setInput(doingNewItem ? "100" : std::to_string(Item::getValue(item)));

    dirty = false;
    applyBut->setColor(sf::Color(20, 240, 50), sf::Color::Black);
    parent->pack(window);
    window->setForceFocus(true);
}

void ItemEditorWindow::makeDirty() {
    dirty = true;
    applyBut->setColor(sf::Color(230, 30, 30), sf::Color::Black);
}

void ItemEditorWindow::onSave() {
    const auto error = [](const std::string& e) {
        bl::dialog::tinyfd_messageBox("Error", e.c_str(), "ok", "error", 1);
    };

    const core::item::Id id = parseInput<core::item::Id>(idEntry);
    const int value         = parseInput<int>(valueEntry);

    if (nameEntry->getInput().empty()) {
        error("Please enter a name");
        return;
    }
    if (descEntry->getInput().empty()) {
        error("Please enter a description");
        return;
    }
    if (idEntry->getInput().empty()) {
        error("Please enter an id");
        return;
    }
    if (valueEntry->getInput().empty()) { error("Please enter a value"); }
    if (openId != id && itemDb.names.find(id) != itemDb.names.end()) {
        if (1 != bl::dialog::tinyfd_messageBox(
                     "Warning", "Id already exists. Overwrite it?", "yesno", "warning", 0)) {
            return;
        }
    }

    itemDb.names[id]        = nameEntry->getInput();
    itemDb.descriptions[id] = descEntry->getInput();
    itemDb.values[id]       = value;

    onChange();
    close();
}

void ItemEditorWindow::onCancel() {
    if (dirty) {
        if (1 != bl::dialog::tinyfd_messageBox(
                     "Warning", "Discard unsaved changes?", "yesno", "warning", 0)) {
            return;
        }
    }
    close();
}

void ItemEditorWindow::close() {
    window->setForceFocus(false);
    window->remove();
}

} // namespace component
} // namespace editor
