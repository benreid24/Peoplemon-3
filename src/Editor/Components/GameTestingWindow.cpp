#include <Editor/Components/GameTestingWindow.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;
namespace
{
constexpr float HourMinutes = 60.f;
constexpr float DayMinutes  = 24.f * HourMinutes;

float timeToFloat(const core::system::Clock::Time& time) {
    const float sum = static_cast<float>(time.hour) * HourMinutes + static_cast<float>(time.minute);
    return sum / DayMinutes;
}

core::system::Clock::Time floatToTime(float t) {
    const unsigned int sum = t * DayMinutes;
    return {sum / 60, sum % 60};
}

std::string itemString(core::item::Id item, unsigned int qty) {
    return std::to_string(static_cast<unsigned int>(item)) + ":" + core::item::Item::getName(item) +
           ":" + std::to_string(qty);
}

std::string pplString(core::pplmn::Id id, unsigned int level) {
    std::stringstream ss;
    ss << static_cast<unsigned int>(id) << ":" << core::pplmn::Peoplemon::name(id)
       << " Lvl: " << level;
    return ss.str();
}

void parsePeoplemon(const std::string& s, core::pplmn::Id& id, unsigned int& level) {
    std::string rd;
    std::stringstream ss(s);
    std::getline(ss, rd, ':');
    id = core::pplmn::Peoplemon::cast(std::atoi(rd.c_str()));
    std::getline(ss, rd, ':');
    std::getline(ss, rd);
    level = std::atoi(rd.c_str());
}

} // namespace

GameTestingWindow::GameTestingWindow(const ActionCb& scb)
: onSave(scb) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Save Editor");
    window->getSignal(Event::Closed).willAlwaysCall(std::bind(&GameTestingWindow::cancel, this));

    LinePacker::Ptr packer = LinePacker::create(LinePacker::Horizontal, 4.f);
    Box::Ptr row           = Box::create(packer);

    nameEntry = TextEntry::create();
    row->pack(Label::create("Name:"), false, true);
    row->pack(nameEntry, true, true);
    window->pack(row, true, false);

    row = Box::create(packer);
    row->pack(Label::create("Money:"), false, true);
    moneyEntry = TextEntry::create();
    moneyEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    row->pack(moneyEntry, true, true);
    window->pack(row, true, false);

    row = Box::create(packer);
    row->pack(Label::create("Time:"), false, true);
    timeSlider = Slider::create(Slider::Direction::Horizontal);
    timeSlider->setRequisition({100.f, 10.f});
    timeSlider->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        updateTimeLabel(floatToTime(timeSlider->getValue()));
    });
    row->pack(timeSlider, true, true);
    timeLabel = Label::create("00:00");
    row->pack(timeLabel, false, true);
    window->pack(row, true, false);

    Label::Ptr section = Label::create("Items");
    section->setCharacterSize(24);
    window->pack(section, true, false);
    row     = Box::create(packer);
    itemBox = SelectBox::create();
    itemBox->setRequisition({250.f, 150.f});
    itemBox->setMaxSize({400.f, 150.f});
    row->pack(itemBox, true, true);
    Box::Ptr column = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    Box::Ptr subrow = Box::create(packer);
    itemSelect      = component::ItemSelector::create();
    subrow->pack(Label::create("Item:"));
    subrow->pack(itemSelect, true, true);
    Button::Ptr addBut = Button::create("Add");
    addBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&GameTestingWindow::addItem, this));
    addBut->setColor(sf::Color::Green, sf::Color::Black);
    subrow->pack(addBut, false, true);
    column->pack(subrow, true, false);
    Button::Ptr rmBut = Button::create("Remove");
    rmBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&GameTestingWindow::rmItem, this));
    rmBut->setColor(sf::Color::Red, sf::Color::Black);
    column->pack(rmBut, false, false);
    row->pack(column, true, true);
    window->pack(row, true, false);

    section = Label::create("Conversation Flags");
    section->setCharacterSize(24);
    window->pack(section, true, false);
    row     = Box::create(packer);
    flagBox = SelectBox::create();
    flagBox->setRequisition({250.f, 150.f});
    flagBox->setMaxSize({400.f, 250.f});
    row->pack(flagBox, true, true);
    column    = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    subrow    = Box::create(packer);
    flagEntry = TextEntry::create();
    subrow->pack(Label::create("Name:"), false, true);
    subrow->pack(flagEntry, true, true);
    column->pack(subrow, true, false);
    subrow          = Box::create(packer);
    Button::Ptr but = Button::create("Add");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&GameTestingWindow::addFlag, this));
    but->setColor(sf::Color::Green, sf::Color::Black);
    subrow->pack(but, false, true);
    but = Button::create("Remove");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&GameTestingWindow::rmFlag, this));
    but->setColor(sf::Color::Red, sf::Color::Black);
    subrow->pack(but, false, true);
    column->pack(subrow, true, false);
    row->pack(column, true, true);
    window->pack(row, true, false);

    section = Label::create("Peoplemon");
    section->setCharacterSize(24);
    window->pack(section, true, false);
    row    = Box::create(packer);
    pplBox = SelectBox::create();
    pplBox->setRequisition({250.f, 150.f});
    pplBox->setMaxSize({400.f, 150.f});
    row->pack(pplBox, true, true);
    column = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    subrow = Box::create(packer);
    subrow->pack(Label::create("Level"), false, true);
    levelSelect = ComboBox::create();
    levelSelect->setMaxHeight(200.f);
    for (int i = 5; i <= 100; i += 5) { levelSelect->addOption(std::to_string(i)); }
    levelSelect->setSelectedOption(9);
    subrow->pack(levelSelect, true, true);
    column->pack(subrow, true, false);
    subrow = Box::create(packer);
    subrow->pack(Label::create("Species"), false, true);
    pplSelect = component::PeoplemonSelector::create();
    pplSelect->setMaxHeight(300.f);
    subrow->pack(pplSelect, true, true);
    column->pack(subrow, true, false);
    subrow = Box::create(packer);
    but    = Button::create("Add");
    but->setColor(sf::Color::Green, sf::Color::Black);
    but->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&GameTestingWindow::addPeoplemon, this));
    subrow->pack(but, false, true);
    but = Button::create("Remove");
    but->setColor(sf::Color::Red, sf::Color::Black);
    but->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&GameTestingWindow::rmPeoplemon, this));
    subrow->pack(but, false, true);
    column->pack(subrow, true, false);
    row->pack(column, true, true);
    window->pack(row, true, false);

    row                 = Box::create(packer);
    Button::Ptr saveBut = Button::create("Save");
    saveBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&GameTestingWindow::doSave, this));
    saveBut->setColor(sf::Color::Green, sf::Color::Black);
    row->pack(saveBut, false, true);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&GameTestingWindow::cancel, this));
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(cancelBut, false, true);
    window->pack(row, true, false);
}

void GameTestingWindow::open(const GUI::Ptr& gui, core::file::GameSave& save) {
    gui->pack(window);
    window->setForceFocus(true);
    activeSave = &save;

    nameEntry->setInput(*save.player.playerName);
    moneyEntry->setInput(std::to_string(*save.player.monei));

    timeSlider->setValue(timeToFloat(*save.clock.time));

    itemBox->clearOptions();
    std::vector<core::player::Bag::Item> items;
    save.player.inventory->getAll(items);
    for (const auto& item : items) { itemBox->addOption(itemString(item.id, item.qty)); }

    flagBox->clearOptions();
    for (const auto& flag : *save.interaction.convFlags) { flagBox->addOption(flag); }

    pplBox->clearOptions();
    for (const auto& ppl : *activeSave->player.peoplemon) {
        pplBox->addOption(pplString(ppl.id(), ppl.currentLevel()));
    }
}

void GameTestingWindow::cancel() {
    window->setForceFocus(false);
    window->remove();
}

void GameTestingWindow::addItem() {
    const core::item::Id item = itemSelect->currentItem();
    if (item != core::item::Id::Unknown) {
        std::vector<std::string> opts;
        itemBox->getAllOptions(opts);
        for (unsigned int i = 0; i < opts.size(); ++i) {
            std::stringstream ss(opts[i] + "\t");
            std::string id, poop, qty;
            std::getline(ss, id, ':');
            std::getline(ss, poop, ':');
            std::getline(ss, qty, '\t');
            if (id == std::to_string(static_cast<unsigned int>(item))) {
                const unsigned int nqty = std::atoi(qty.c_str()) + 1;
                itemBox->editOptionText(i, itemString(item, nqty));
                return;
            }
        }

        // not found, add
        itemBox->addOption(itemString(item, 1));
    }
}

void GameTestingWindow::rmItem() {
    if (itemBox->getSelectedOption().has_value()) {
        itemBox->removeOption(itemBox->getSelectedOption().value());
    }
}

void GameTestingWindow::addFlag() {
    if (!flagEntry->getInput().empty()) { flagBox->addOption(flagEntry->getInput()); }
}

void GameTestingWindow::rmFlag() {
    if (flagBox->getSelectedOption().has_value()) {
        flagBox->removeOption(flagBox->getSelectedOption().value());
    }
}

void GameTestingWindow::doSave() {
    if (nameEntry->getInput().empty()) {
        bl::dialog::tinyfd_messageBox("Error", "Enter a name", "ok", "error", 0);
        return;
    }

    *activeSave->player.playerName = nameEntry->getInput();
    activeSave->saveName           = *activeSave->player.playerName;
    *activeSave->player.monei      = std::atol(moneyEntry->getInput().c_str());
    *activeSave->player.sex        = bl::util::Random::get<core::player::Gender>(
        core::player::Gender::Boy, core::player::Gender::Girl);
    *activeSave->clock.time = floatToTime(timeSlider->getValue());

    activeSave->player.inventory->clear();
    std::vector<std::string> vec;
    itemBox->getAllOptions(vec);
    for (const auto& s : vec) {
        std::stringstream ss(s + "\t");
        std::string id, poop, qtys;
        std::getline(ss, id, ':');
        std::getline(ss, poop, ':');
        std::getline(ss, qtys, '\t');

        const core::item::Id item = core::item::Item::cast(std::atoi(id.c_str()));
        const unsigned int qty    = std::atoi(qtys.c_str());
        activeSave->player.inventory->addItem(item, qty);
    }

    flagBox->getAllOptions(vec);
    activeSave->interaction.convFlags->clear();
    for (const auto& s : vec) { activeSave->interaction.convFlags->emplace(s); }

    activeSave->player.peoplemon->clear();
    for (unsigned int i = 0; i < pplBox->optionCount(); ++i) {
        core::pplmn::Id id;
        unsigned int lvl;
        parsePeoplemon(pplBox->getOption(i), id, lvl);
        activeSave->player.peoplemon->emplace_back(id, lvl);
    }

    activeSave->editorSave();
    onSave();
    cancel();
}

void GameTestingWindow::updateTimeLabel(const core::system::Clock::Time& time) {
    std::string minute = std::to_string(time.minute);
    if (minute.size() == 1) minute.insert(0, "0");
    timeLabel->setText(std::to_string(time.hour) + ":" + minute);
}

void GameTestingWindow::addPeoplemon() {
    if (pplBox->optionCount() < 6 && pplSelect->currentPeoplemon() != core::pplmn::Id::Unknown) {
        pplBox->addOption(
            pplString(pplSelect->currentPeoplemon(), levelSelect->getSelectedOption() * 5 + 5));
    }
}

void GameTestingWindow::rmPeoplemon() {
    const auto i = pplBox->getSelectedOption();
    if (i.has_value()) { pplBox->removeOption(i.value()); }
}

} // namespace component
} // namespace editor
