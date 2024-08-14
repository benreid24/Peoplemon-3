#include <Editor/Components/CatchRegionWindow.hpp>

#include <iomanip>
#include <numeric>

namespace editor
{
namespace component
{
namespace
{
constexpr float FreqRange = static_cast<float>(std::numeric_limits<std::uint16_t>::max() - 1);

bool isnum(const std::string& n) {
    for (char c : n) {
        if (!std::isdigit(c)) return false;
    }
    if (n.empty()) return false;
    return true;
}
} // namespace

using namespace bl::gui;

CatchRegionWindow::CatchRegionWindow(const TriggerCb& cb)
: onSave(cb) {
    window =
        Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Peoplemon Region Editor");
    window->getSignal(Event::Closed).willAlwaysCall(std::bind(&CatchRegionWindow::close, this));
    window->setRequisition({700.f, 600.f});

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Name:"), false, true);
    nameEntry = TextEntry::create();
    row->pack(nameEntry, true, true);
    window->pack(row, true, false);

    peopleScroll = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 8.f));
    peopleScroll->setMaxSize({700.f, 550.f});
    window->pack(peopleScroll, true, true);

    row                = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    Button::Ptr addBut = Button::create("Add Peoplemon");
    addBut->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&CatchRegionWindow::add, this));
    row->pack(addBut, false, true);

    Box::Ptr subrow       = Box::create(LinePacker::create(
        LinePacker::Horizontal, 4.f, LinePacker::Compact, LinePacker::RightAlign));
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    cancelBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&CatchRegionWindow::close, this));
    subrow->pack(cancelBut, false, true);
    Button::Ptr saveBut = Button::create("Save");
    saveBut->setColor(sf::Color(3, 252, 244), sf::Color::Black);
    saveBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&CatchRegionWindow::save, this));
    subrow->pack(saveBut, false, true);
    row->pack(subrow, true, true);
    window->pack(row, true, false);
}

void CatchRegionWindow::open(bl::gui::GUI* gui, const core::map::CatchRegion& v) {
    nameEntry->setInput(v.name);
    rows.reserve(v.wilds.size());
    for (const core::pplmn::WildPeoplemon& wild : v.wilds) {
        rows.emplace_back(std::bind(&CatchRegionWindow::freqChange, this, std::placeholders::_1),
                          std::bind(&CatchRegionWindow::remove, this, std::placeholders::_1));
        rows.back().idSelect->setPeoplemon(wild.id);
        rows.back().minLevel->setInput(std::to_string(wild.minLevel));
        rows.back().maxLevel->setInput(std::to_string(wild.maxLevel));
        rows.back().freqSlider->setValue(static_cast<float>(wild.frequency) / FreqRange);
        peopleScroll->pack(rows.back().row, true, false);
    }

    gui->pack(window);
    window->setForceFocus(true);

    value = v;
}

const core::map::CatchRegion& CatchRegionWindow::getValue() {
    value.name = nameEntry->getInput();
    value.wilds.clear();
    value.wilds.reserve(rows.size());
    for (const Row& row : rows) {
        value.wilds.emplace_back();
        value.wilds.back().id       = row.idSelect->currentPeoplemon();
        value.wilds.back().minLevel = std::atoi(row.minLevel->getInput().c_str());
        value.wilds.back().maxLevel = std::atoi(row.maxLevel->getInput().c_str());
        value.wilds.back().frequency =
            static_cast<std::uint16_t>(row.freqSlider->getValue() * FreqRange);
    }

    return value;
}

void CatchRegionWindow::close() {
    window->setForceFocus(false);
    window->remove();
    for (auto& row : rows) { row.row->remove(); }
    rows.clear();
}

CatchRegionWindow::Row::Row(const Callback& cb, const Callback& delcb) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    row->setColor(sf::Color(119, 252, 35, 100), sf::Color::Green);
    row->setOutlineThickness(1.f);

    Box::Ptr col = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    col->pack(Label::create("Peoplemon"));
    idSelect = PeoplemonSelector::create();
    idSelect->setVerticalAlignment(RenderSettings::Bottom);
    idSelect->setMaxHeight(300.f);
    col->pack(idSelect, true, false);
    row->pack(col, false, true);

    col = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    col->pack(Label::create("Min Lvl"), true, false);
    minLevel = TextEntry::create();
    minLevel->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    col->pack(minLevel, true, false);
    row->pack(col, false, true);

    col = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    col->pack(Label::create("Max Lvl"), true, false);
    maxLevel = TextEntry::create();
    maxLevel->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    col->pack(maxLevel, true, false);
    row->pack(col, false, true);

    col             = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    Box::Ptr subrow = Box::create(LinePacker::create(LinePacker::Horizontal, 10.f));
    freqLabel       = Label::create("Freq: 0%");
    freqLabel->setRequisition({90.f, 10.f});
    lockBut = CheckButton::create("Lock");
    freqLabel->setHorizontalAlignment(RenderSettings::Left);
    subrow->pack(freqLabel, true, true);
    subrow->pack(lockBut, false, true);
    col->pack(subrow, true, false);
    freqSlider = Slider::create(Slider::Horizontal);
    freqSlider->getSignal(Event::ValueChanged).willAlwaysCall(std::bind(cb, std::placeholders::_2));
    freqSlider->setRequisition({200.f, 20.f});
    freqSlider->setSliderIncrement(0.001f);
    col->pack(freqSlider, true, false);
    row->pack(col, false, true);

    col    = Box::create(LinePacker::create(
        LinePacker::Vertical, 4.f, LinePacker::Compact, LinePacker::BottomAlign));
    delBut = Button::create("Remove");
    delBut->setColor(sf::Color::Red, sf::Color::Black);
    delBut->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(delcb, std::placeholders::_2));
    col->pack(delBut);
    row->pack(col, false, true);
}

void CatchRegionWindow::save() {
    if (validate()) {
        onSave();
        close();
    }
}

void CatchRegionWindow::add() {
    unsigned int unlocked = 1;
    float unlockedAmount  = 0.f;
    for (const auto& row : rows) {
        if (!row.lockBut->getValue()) {
            ++unlocked;
            unlockedAmount += row.freqSlider->getValue();
        }
    }
    const float avg = unlockedAmount / static_cast<float>(unlocked);

    rows.emplace_back(std::bind(&CatchRegionWindow::freqChange, this, std::placeholders::_1),
                      std::bind(&CatchRegionWindow::remove, this, std::placeholders::_1));
    rows.back().minLevel->setInput("20");
    rows.back().maxLevel->setInput("30");
    peopleScroll->pack(rows.back().row, true, false);

    if (rows.size() == 1) { rows.front().freqSlider->setValue(1.f); }
    else {
        for (auto& row : rows) {
            if (!row.lockBut->getValue()) {
                row.freqSlider->setValue(avg, false);
                std::stringstream ss;
                ss << "Freq: ";
                ss << std::setprecision(5) << (row.freqSlider->getValue() * 100.f) << "%";
                row.freqLabel->setText(ss.str());
            }
        }
    }
}

void CatchRegionWindow::remove(bl::gui::Element* e) {
    for (auto it = rows.begin(); it != rows.end(); ++it) {
        if (static_cast<Element*>(it->delBut.get()) == e) {
            it->row->remove();
            rows.erase(it);
            return;
        }
    }

    unsigned int unlocked = 0;
    float unlockedAmount  = 0.f;
    for (const auto& row : rows) {
        if (!row.lockBut->getValue()) {
            ++unlocked;
            unlockedAmount += row.freqSlider->getValue();
        }
    }
    const float avg = unlockedAmount / static_cast<float>(unlocked);

    for (auto& row : rows) {
        if (!row.lockBut->getValue()) {
            row.freqSlider->setValue(avg, false);
            std::stringstream ss;
            ss << "Freq: ";
            ss << std::setprecision(5) << (row.freqSlider->getValue() * 100.f) << "%";
            row.freqLabel->setText(ss.str());
        }
    }
}

void CatchRegionWindow::freqChange(Element* e) {
    float sum             = 0.f;
    unsigned int unlocked = 0;
    for (const auto& row : rows) {
        sum += row.freqSlider->getValue();
        if (static_cast<Element*>(row.freqSlider.get()) != e && !row.lockBut->getValue()) {
            ++unlocked;
        }
    }

    const float diff   = 1.f - sum;
    const float shared = diff / static_cast<float>(unlocked);

    for (auto& row : rows) {
        if (static_cast<Element*>(row.freqSlider.get()) != e && !row.lockBut->getValue()) {
            float f = row.freqSlider->getValue() + shared;
            if (f <= 0.f) f = 0.001f;
            if (f > 1.f) f = 1.f;
            row.freqSlider->setValue(f, false);
        }
        std::stringstream ss;
        ss << "Freq: ";
        ss << std::setprecision(5) << (row.freqSlider->getValue() * 100.f) << "%";
        row.freqLabel->setText(ss.str());
    }
}

bool CatchRegionWindow::validate() const {
    if (rows.empty()) {
        bl::dialog::tinyfd_messageBox(
            "Error", "Region must have at least one Peoplemon", "ok", "error", 1);
        return false;
    }
    if (nameEntry->getInput().empty()) {
        bl::dialog::tinyfd_messageBox("Error", "Please enter a name", "ok", "error", 1);
        return false;
    }

    float sum = 0.f;
    for (const Row& row : rows) {
        if (row.idSelect->currentPeoplemon() == core::pplmn::Id::Unknown) {
            bl::dialog::tinyfd_messageBox("Error", "Peoplemon cannot be blank", "ok", "error", 1);
            return false;
        }
        if (!isnum(row.minLevel->getInput()) || !isnum(row.maxLevel->getInput())) {
            bl::dialog::tinyfd_messageBox("Error", "Levels must be numbers", "ok", "error", 1);
            return false;
        }
        const int mn = std::atoi(row.minLevel->getInput().c_str());
        const int mx = std::atoi(row.maxLevel->getInput().c_str());
        if (mn <= 0 || mx <= 0) {
            bl::dialog::tinyfd_messageBox("Error", "Levels must be non-zero", "ok", "error", 1);
            return false;
        }
        if (mn > 100 || mx > 100) {
            bl::dialog::tinyfd_messageBox(
                "Error", "Levels must be less than or equal to 100", "ok", "error", 1);
            return false;
        }
        if (mx < mn) {
            bl::dialog::tinyfd_messageBox(
                "Error", "Min level must be below max level", "ok", "error", 1);
            return false;
        }
        sum += row.freqSlider->getValue();
    }

    if (std::abs(1.f - sum) > 0.001f) {
        bl::dialog::tinyfd_messageBox("Error", "Frequencies must sum to 100%", "ok", "error", 1);
        return false;
    }

    return true;
}

} // namespace component
} // namespace editor
