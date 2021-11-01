#include <Editor/Pages/Subpages/Layers.hpp>

#include <BLIB/Util/Random.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Layers::Layers(const AppendCb& bottomAddCb, const AppendCb& ysortAddCb, const AppendCb& topAddCb,
               const DeleteCb& delCb, const ShiftCb& shiftCb, const RenderFilterCb& filterCb)
: bottomAdd(bottomAddCb)
, ysortAdd(ysortAddCb)
, topAdd(topAddCb)
, delCb(delCb)
, shiftCb(shiftCb)
, filterCb(filterCb) {
    contentWrapper = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    contentWrapper->setOutlineThickness(0.f);
    content = Notebook::create();
    content->setRequisition({1.f, 200.f});
}

void Layers::sync(const std::vector<core::map::LayerSet>& levels,
                  const std::vector<std::vector<bool>>& filter) {
    for (unsigned int i = 0; i < content->pageCount(); ++i) { content->removePageByIndex(i); }

    pages.clear();
    pages.reserve(levels.size());
    for (unsigned int i = 0; i < levels.size(); ++i) {
        const auto& level = levels[i];
        pages.emplace_back(
            i, level, filter[i], filterCb, bottomAdd, ysortAdd, topAdd, delCb, shiftCb);
        const std::string title = "Level " + std::to_string(pages.back().index);
        content->addPage(title, title, pages.back().page);
    }
}

Box::Ptr Layers::getContent() { return contentWrapper; }

void Layers::pack() { contentWrapper->pack(content, true, true); }

void Layers::unpack() { content->remove(); }

Layers::LevelTab::LevelTab(unsigned int i, const core::map::LayerSet& level,
                           const std::vector<bool>& filter, const RenderFilterCb& vcb,
                           const AppendCb& bottomAddCb, const AppendCb& ysortAddCb,
                           const AppendCb& topAddCb, const DeleteCb& delCb, const ShiftCb& shiftCb)
: index(i) {
    page = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 8));
    page->setMaxSize({300, 175});

    Box::Ptr box         = Box::create(LinePacker::create(LinePacker::Vertical, 8));
    const auto visibleCb = [this, vcb](unsigned int layer, bool v) { vcb(index, layer, v); };
    const auto deleteCb  = [this, delCb](unsigned int layer) { delCb(index, layer); };
    const auto onShift   = [this, shiftCb](unsigned int layer, bool up) {
        shiftCb(index, layer, up);
    };

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 0, LinePacker::Uniform));
    Button::Ptr addBut = Button::create("Add");
    addBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(
            [this, bottomAddCb, visibleCb](const Event&, Element*) { bottomAddCb(index); });
    bottomBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row->pack(Label::create("Bottom Layers"), true, true);
    row->pack(addBut, false, false);
    bottomBox->pack(row, true, false);

    row    = Box::create(LinePacker::create(LinePacker::Horizontal, 0, LinePacker::Uniform));
    addBut = Button::create("Add");
    addBut->getSignal(Event::LeftClicked)
        .willAlwaysCall([this, ysortAddCb](const Event&, Element*) { ysortAddCb(index); });
    ysortBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row->pack(Label::create("Y-Sort Layers"), true, true);
    row->pack(addBut, false, false);
    ysortBox->pack(row, true, false);

    row    = Box::create(LinePacker::create(LinePacker::Horizontal, 0, LinePacker::Uniform));
    addBut = Button::create("Add");
    addBut->getSignal(Event::LeftClicked).willAlwaysCall([this, topAddCb](const Event&, Element*) {
        topAddCb(index);
    });
    topBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row->pack(Label::create("Top Layers"), true, true);
    row->pack(addBut, false, false);
    topBox->pack(row, true, false);

    box->pack(bottomBox, true, false);
    box->pack(ysortBox, true, false);
    box->pack(topBox, true, false);

    items.reserve(level.layerCount());
    for (unsigned int i = 0; i < level.bottomLayers().size(); ++i) {
        items.emplace_back(i, i != 0, true, filter[i], visibleCb, deleteCb, onShift);
        bottomBox->pack(items.back().row, true, false);
    }
    for (unsigned int i = level.bottomLayers().size();
         i < level.bottomLayers().size() + level.ysortLayers().size();
         ++i) {
        items.emplace_back(i, true, true, filter[i], visibleCb, deleteCb, onShift);
        ysortBox->pack(items.back().row, true, false);
    }
    for (unsigned int i = level.bottomLayers().size() + level.ysortLayers().size();
         i < level.layerCount();
         ++i) {
        items.emplace_back(
            i, true, i != level.layerCount() - 1, filter[i], visibleCb, deleteCb, onShift);
        topBox->pack(items.back().row, true, false);
    }

    page->pack(box, true, true);
}

Layers::LayerRow::LayerRow(unsigned int i, bool canUp, bool canDown, bool visible,
                           const VisibleCb& visibleCb, const DeleteCb& delCb,
                           const ShiftCb& shiftCb)
: index(i) {
    row = Box::create(LinePacker::create(
        LinePacker::Horizontal, 2.f, LinePacker::Compact, LinePacker::LeftAlign));
    row->setOutlineThickness(1.f);
    row->setColor(sf::Color::Transparent, sf::Color::Red);

    name = Label::create("Layer " + std::to_string(i));
    name->setColor(sf::Color(bl::util::Random::get<std::uint8_t>(80, 255),
                             bl::util::Random::get<std::uint8_t>(80, 255),
                             bl::util::Random::get<std::uint8_t>(80, 255)),
                   sf::Color::Transparent);
    row->pack(name, true, false);

    visibleToggle = CheckButton::create("Visible");
    visibleToggle->setValue(visible);
    visibleToggle->getSignal(Event::ValueChanged)
        .willAlwaysCall([this, visibleCb](const Event&, Element*) {
            visibleCb(index, visibleToggle->getValue());
        });
    row->pack(visibleToggle, false, true);

    upBut = Button::create("Up");
    upBut->setTooltip("Move this layer up the list. Higher layers are below lower layers");
    upBut->setActive(canUp);
    if (canUp) {
        upBut->getSignal(Event::LeftClicked)
            .willAlwaysCall([this, shiftCb](const Event&, Element*) { shiftCb(index, true); });
    }
    row->pack(upBut, false, true);

    downBut = Button::create("Down");
    downBut->setTooltip("Move this layer down the list. Lower layers are on top of higher layers");
    downBut->setActive(canDown);
    if (canDown) {
        downBut->getSignal(Event::LeftClicked)
            .willAlwaysCall([this, shiftCb](const Event&, Element*) { shiftCb(index, false); });
    }
    row->pack(downBut, false, true);

    delBut = Button::create("Delete");
    delBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    delBut->getSignal(Event::LeftClicked).willAlwaysCall([this, delCb](const Event&, Element*) {
        delCb(index);
    });
    row->pack(delBut, false, true);
}

} // namespace page
} // namespace editor
