#include <Editor/Components/StatBox.hpp>

namespace editor
{
namespace component
{
namespace
{
constexpr float InputWidth = 60.f;
}
using namespace bl::gui;

StatBox::StatBox(Mode m)
: mode(m) {
    const auto f = std::bind(&StatBox::onChange, this, std::placeholders::_2);

    hpEntry = TextEntry::create();
    hpEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    hpEntry->setRequisition({InputWidth, 1.f});
    hpEntry->getSignal(Event::ValueChanged).willAlwaysCall(f);

    atkEntry = TextEntry::create();
    atkEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    atkEntry->setRequisition({InputWidth, 1.f});
    atkEntry->getSignal(Event::ValueChanged).willAlwaysCall(f);

    defEntry = TextEntry::create();
    defEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    defEntry->setRequisition({InputWidth, 1.f});
    defEntry->getSignal(Event::ValueChanged).willAlwaysCall(f);

    spAtkEntry = TextEntry::create();
    spAtkEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    spAtkEntry->setRequisition({InputWidth, 1.f});
    spAtkEntry->getSignal(Event::ValueChanged).willAlwaysCall(f);

    spDefEntry = TextEntry::create();
    spDefEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    spDefEntry->setRequisition({InputWidth, 1.f});
    spDefEntry->getSignal(Event::ValueChanged).willAlwaysCall(f);

    spdEntry = TextEntry::create();
    spdEntry->setMode(TextEntry::Mode::Integer | TextEntry::Mode::Unsigned);
    spdEntry->setRequisition({InputWidth, 1.f});
    spdEntry->getSignal(Event::ValueChanged).willAlwaysCall(f);

    reset();
}

void StatBox::reset() {
    hpEntry->setInput("0");
    atkEntry->setInput("0");
    defEntry->setInput("0");
    spAtkEntry->setInput("0");
    spDefEntry->setInput("0");
    spdEntry->setInput("0");
}

core::pplmn::Stats StatBox::currentValue() const {
    core::pplmn::Stats ret;
    ret.hp    = std::atoi(hpEntry->getInput().c_str());
    ret.atk   = std::atoi(atkEntry->getInput().c_str());
    ret.def   = std::atoi(defEntry->getInput().c_str());
    ret.spatk = std::atoi(spAtkEntry->getInput().c_str());
    ret.spdef = std::atoi(spDefEntry->getInput().c_str());
    ret.spd   = std::atoi(spdEntry->getInput().c_str());
    return ret;
}

void StatBox::update(const core::pplmn::Stats& val) {
    hpEntry->setInput(std::to_string(val.hp));
    atkEntry->setInput(std::to_string(val.atk));
    defEntry->setInput(std::to_string(val.def));
    spAtkEntry->setInput(std::to_string(val.spatk));
    spDefEntry->setInput(std::to_string(val.spdef));
    spdEntry->setInput(std::to_string(val.spd));
}

void StatBox::pack(Box& content) {
    LinePacker::Ptr rowPacker = LinePacker::create(LinePacker::Horizontal, 4.f);

    Box::Ptr row   = Box::create(rowPacker);
    Label::Ptr lbl = Label::create("HP:");
    lbl->setHorizontalAlignment(RenderSettings::Left);
    row->pack(lbl, true, true);
    row->pack(hpEntry, false, true);
    content.pack(row, true, false);

    row = Box::create(rowPacker);
    lbl = Label::create("Atk:");
    lbl->setHorizontalAlignment(RenderSettings::Left);
    row->pack(lbl, true, true);
    row->pack(atkEntry, false, true);
    content.pack(row, true, false);

    row = Box::create(rowPacker);
    lbl = Label::create("Def:");
    lbl->setHorizontalAlignment(RenderSettings::Left);
    row->pack(lbl, true, true);
    row->pack(defEntry, false, true);
    content.pack(row, true, false);

    row = Box::create(rowPacker);
    lbl = Label::create("SpAtk:");
    lbl->setHorizontalAlignment(RenderSettings::Left);
    row->pack(lbl, true, true);
    row->pack(spAtkEntry, false, true);
    content.pack(row, true, false);

    row = Box::create(rowPacker);
    lbl = Label::create("SpDef:");
    lbl->setHorizontalAlignment(RenderSettings::Left);
    row->pack(lbl, true, true);
    row->pack(spDefEntry, false, true);
    content.pack(row, true, false);

    row = Box::create(rowPacker);
    lbl = Label::create("Spd:");
    lbl->setHorizontalAlignment(RenderSettings::Left);
    row->pack(lbl, true, true);
    row->pack(spdEntry, false, true);
    content.pack(row, true, false);
}

void StatBox::onChange(Element* e) {
    using core::pplmn::Stats;

    TextEntry* te = dynamic_cast<TextEntry*>(e);
    if (te) {
        const Stats val = currentValue();
        if (mode == EV) {
            if (val.sum() > Stats::MaxEVSum) {
                std::stringstream ss;
                ss << "EV values cannot sum to more than " << Stats::MaxEVSum;
                bl::dialog::tinyfd_messageBox("Warning", ss.str().c_str(), "warning", "ok", 1);
            }
            for (const core::pplmn::Stat stat : Stats::IterableStats) {
                if (val.get(stat) > Stats::MaxEVStat) {
                    std::stringstream ss;
                    ss << Stats::statToString(stat) << " EV stat cannot be above "
                       << Stats::MaxEVStat;
                    bl::dialog::tinyfd_messageBox("Warning", ss.str().c_str(), "warning", "ok", 1);
                }
            }
        }
        else {
            for (const core::pplmn::Stat stat : Stats::IterableStats) {
                if (val.get(stat) > Stats::MaxIVStat) {
                    std::stringstream ss;
                    ss << Stats::statToString(stat) << " IV stat cannot be above "
                       << Stats::MaxIVStat;
                    bl::dialog::tinyfd_messageBox("Warning", ss.str().c_str(), "warning", "ok", 1);
                }
            }
        }
    }
}

} // namespace component
} // namespace editor
