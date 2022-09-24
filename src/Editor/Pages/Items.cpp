#include <Editor/Pages/Items.hpp>

namespace editor
{
namespace page
{
namespace
{
std::vector<core::item::Id> getSorted(const core::file::ItemDB& db) {
    std::vector<core::item::Id> ids;
    ids.reserve(db.names.size());
    for (const auto& p : db.names) { ids.emplace_back(p.first); }
    std::sort(ids.begin(), ids.end());
    return ids;
}
} // namespace

using namespace bl::gui;

Items::Items(core::system::Systems& s)
: Page(s)
, itemWindow(itemDb, std::bind(&Items::onChange, this)) {
    content = Box::create(LinePacker::create(LinePacker::Vertical));

    Box::Ptr row    = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    Button::Ptr but = Button::create("New Item");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Items::newItem, this));
    row->pack(but, false, true);
    saveBut = Button::create("Save");
    saveBut->setColor(sf::Color::Green, sf::Color::Black);
    saveBut->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Items::save, this));
    row->pack(saveBut, false, true);
    but = Button::create("Reset");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Items::reset, this));
    but->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(but, false, true);
    content->pack(row, true, false);

    rowArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4.f));
    content->pack(rowArea, true, true);

    if (!itemDb.load()) BL_LOG_ERROR << "Failed to load item database";
    core::item::Item::setDataSource(itemDb);
    sync();
}

void Items::sync() {
    rowArea->clearChildren(true);
    const std::vector<core::item::Id> allIds = getSorted(itemDb);

    int i                   = 0;
    LinePacker::Ptr rowPack = LinePacker::create(LinePacker::Horizontal, 4.f, LinePacker::Uniform);
    for (core::item::Id id : allIds) {
        Box::Ptr row = Box::create(rowPack);
        row->setColor(i % 2 == 0 ? sf::Color(185, 185, 185) : sf::Color(70, 70, 70),
                      sf::Color(20, 85, 230));
        Label::Ptr lbl = Label::create("Id: " + std::to_string(static_cast<int>(id)));
        lbl->setColor(sf::Color(30, 85, 255), sf::Color::Transparent);
        row->pack(lbl, false, true);
        lbl = Label::create(core::item::Item::getName(id));
        lbl->setColor(sf::Color(255, 60, 90), sf::Color::Transparent);
        row->pack(lbl, false, true);
        lbl = Label::create("Value: " + std::to_string(core::item::Item::getValue(id)));
        lbl->setColor(sf::Color(60, 255, 90), sf::Color::Transparent);
        row->pack(lbl, false, true);
        Button::Ptr but = Button::create("Edit");
        but->setColor(sf::Color(230, 230, 30), sf::Color::Black);
        but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Items::editItem, this, id));
        row->pack(but, false, true);
        but = Button::create("Delete");
        but->setColor(sf::Color(230, 30, 30), sf::Color::Black);
        but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Items::deleteItem, this, id));
        row->pack(but, false, true);
        rowArea->pack(row, true, false);
        ++i;
    }
}

void Items::makeDirty() { saveBut->setColor(sf::Color::Yellow, sf::Color::Black); }

void Items::save() {
    if (!itemDb.save()) { BL_LOG_ERROR << "Failed to save item database"; }
    else {
        saveBut->setColor(sf::Color::Green, sf::Color::Black);
    }
}

void Items::newItem() { itemWindow.open(parent, core::item::Id::Unknown); }

void Items::editItem(core::item::Id item) { itemWindow.open(parent, item); }

void Items::reset() {
    if (!itemDb.load()) { BL_LOG_ERROR << "Failed to load item database"; }
    else {
        sync();
        saveBut->setColor(sf::Color::Green, sf::Color::Black);
    }
}

void Items::deleteItem(core::item::Id item) {
    makeDirty();

    itemDb.names.erase(item);
    itemDb.descriptions.erase(item);
    itemDb.values.erase(item);

    sync();
}

void Items::onChange() {
    sync();
    makeDirty();
}

void Items::update(float) {}

} // namespace page
} // namespace editor
