#include <Editor/Pages/Peoplemon.hpp>

#include <Core/Resources.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Peoplemon::Peoplemon(core::system::Systems& s)
: Page(s)
, window(peoplemonDb, std::bind(&Peoplemon::onChange, this))
, firstSync(false) {
    content = Box::create(LinePacker::create(LinePacker::Vertical));

    Box::Ptr row    = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    Button::Ptr but = Button::create("New Peoplemon");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Peoplemon::newPeoplemon, this));
    row->pack(but, false, true);
    saveBut = Button::create("Save");
    saveBut->setColor(sf::Color::Green, sf::Color::Black);
    saveBut->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Peoplemon::save, this));
    row->pack(saveBut, false, true);
    but = Button::create("Reset");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Peoplemon::reset, this));
    but->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(but, false, true);
    content->pack(row, true, false);

    rowArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4.f));
    content->pack(rowArea, true, true);
}

void Peoplemon::update(float) {
    if (!firstSync) {
        firstSync = true;
        if (!peoplemonDb.load()) { BL_LOG_ERROR << "Failed to load Peoplemon database"; }
        core::pplmn::Peoplemon::setDataSource(peoplemonDb);
        sync();
    }
}

void Peoplemon::sync() {
    rowArea->clearChildren(true);
    const auto& allIds = core::pplmn::Peoplemon::validIds();

    int i                   = 0;
    LinePacker::Ptr rowPack = LinePacker::create(LinePacker::Horizontal, 4.f, LinePacker::Uniform);
    for (core::pplmn::Id id : allIds) {
        Box::Ptr row = Box::create(rowPack);
        row->setColor(i % 2 == 0 ? sf::Color(185, 185, 185) : sf::Color(70, 70, 70),
                      sf::Color(20, 85, 230));
        Label::Ptr lbl = Label::create("Id: " + std::to_string(static_cast<int>(id)));
        lbl->setColor(sf::Color(30, 85, 255), sf::Color::Transparent);
        row->pack(lbl, false, true);

        Image::Ptr icon =
            Image::create(ImageManager::load(core::pplmn::Peoplemon::thumbnailImage(id)));
        icon->scaleToSize({40.f, 40.f});
        row->pack(icon);

        lbl = Label::create(core::pplmn::Peoplemon::name(id));
        lbl->setColor(sf::Color(255, 60, 90), sf::Color::Transparent);
        row->pack(lbl, false, true);
        Button::Ptr but = Button::create("Edit");
        but->setColor(sf::Color(230, 230, 30), sf::Color::Black);
        but->getSignal(Event::LeftClicked)
            .willAlwaysCall(std::bind(&Peoplemon::editPeoplemon, this, id));
        row->pack(but, false, true);
        but = Button::create("Delete");
        but->setColor(sf::Color(230, 30, 30), sf::Color::Black);
        but->getSignal(Event::LeftClicked)
            .willAlwaysCall(std::bind(&Peoplemon::deletePeoplemon, this, id));
        row->pack(but, false, true);
        rowArea->pack(row, true, false);
        ++i;
    }
}

void Peoplemon::makeDirty() { saveBut->setColor(sf::Color::Yellow, sf::Color::Black); }

void Peoplemon::save() {
    if (!peoplemonDb.save()) { BL_LOG_ERROR << "Failed to save peoplemon database"; }
    else { saveBut->setColor(sf::Color::Green, sf::Color::Black); }
}

void Peoplemon::newPeoplemon() { window.open(parent, core::pplmn::Id::Unknown); }

void Peoplemon::editPeoplemon(core::pplmn::Id ppl) { window.open(parent, ppl); }

void Peoplemon::reset() {
    if (!peoplemonDb.load()) { BL_LOG_ERROR << "Failed to load peoplemon database"; }
    else {
        sync();
        saveBut->setColor(sf::Color::Green, sf::Color::Black);
    }
}

void Peoplemon::deletePeoplemon(core::pplmn::Id id) {
    makeDirty();

    peoplemonDb.names.erase(id);
    peoplemonDb.descriptions.erase(id);
    peoplemonDb.types.erase(id);
    peoplemonDb.abilities.erase(id);
    peoplemonDb.stats.erase(id);
    peoplemonDb.validMoves.erase(id);
    peoplemonDb.learnedMoves.erase(id);
    peoplemonDb.evolveLevels.erase(id);
    peoplemonDb.evolveIds.erase(id);
    peoplemonDb.evAwards.erase(id);
    peoplemonDb.xpGroups.erase(id);
    peoplemonDb.xpMults.erase(id);
    peoplemonDb.catchRates.erase(id);

    sync();
}

void Peoplemon::onChange() {
    sync();
    makeDirty();
}

} // namespace page
} // namespace editor
