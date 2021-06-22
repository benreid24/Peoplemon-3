#include <Editor/Pages/Subpages/Layers.hpp>

#include <BLIB/Util/Random.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Layers::Layers(const AppendCb& bottomAddCb, const AppendCb& ysortAddCb, const AppendCb& topAddCb,
               const DeleteCb& delCb, const RenderFilterCb& filterCb)
: bottomAdd(bottomAddCb)
, ysortAdd(ysortAddCb)
, topAdd(topAddCb)
, delCb(delCb)
, filterCb(filterCb) {
    contentWrapper = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    content        = Notebook::create();
}

void Layers::sync(const std::vector<core::map::LayerSet>& levels) {
    for (unsigned int i = 0; i < content->pageCount(); ++i) { content->removePageByIndex(i); }

    pages.clear();
    pages.reserve(levels.size());
    for (unsigned int i = 0; i < levels.size(); ++i) {
        const auto& level = levels[i];
        pages.emplace_back(i, level, filterCb, bottomAdd, ysortAdd, topAdd);
        const std::string title = "Level " + std::to_string(pages.back().index);
        content->addPage(title, title, pages.back().page);
    }
}

Box::Ptr Layers::getContent() { return contentWrapper; }

void Layers::pack() { contentWrapper->pack(content, true, true); }

void Layers::unpack() { content->remove(); }

Layers::LevelTab::LevelTab(unsigned int i, const core::map::LayerSet& level,
                           const RenderFilterCb& vcb, const AppendCb& bottomAddCb,
                           const AppendCb& ysortAddCb, const AppendCb& topAddCb)
: index(i) {
    page = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 8));
    page->setMaxSize({300, 175});

    Box::Ptr box = Box::create(LinePacker::create(LinePacker::Vertical, 8));

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 0, LinePacker::Uniform));
    Button::Ptr addBut = Button::create("Add");
    addBut->getSignal(Action::LeftClicked)
        .willAlwaysCall([this, bottomAddCb](const Action&, Element*) { bottomAddCb(index); });
    bottomBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row->pack(Label::create("Bottom Layers"), true, false);
    row->pack(addBut, false, false);
    bottomBox->pack(row, true, false);

    row    = Box::create(LinePacker::create(LinePacker::Horizontal, 0, LinePacker::Uniform));
    addBut = Button::create("Add");
    addBut->getSignal(Action::LeftClicked)
        .willAlwaysCall([this, ysortAddCb](const Action&, Element*) { ysortAddCb(index); });
    ysortBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row->pack(Label::create("Y-Sort Layers"), true, true);
    row->pack(addBut, false, false);
    ysortBox->pack(row, true, false);

    row    = Box::create(LinePacker::create(LinePacker::Horizontal, 0, LinePacker::Uniform));
    addBut = Button::create("Add");
    addBut->getSignal(Action::LeftClicked)
        .willAlwaysCall([this, topAddCb](const Action&, Element*) { topAddCb(index); });
    topBox = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row->pack(Label::create("Top Layers"), true, true);
    row->pack(addBut, false, false);
    topBox->pack(row, true, false);

    box->pack(bottomBox, true, false);
    box->pack(ysortBox, true, false);
    box->pack(topBox, true, false);

    const auto visibleCb = [this, vcb](unsigned int layer, bool v) { vcb(index, layer, v); };

    items.reserve(level.layerCount());
    for (unsigned int i = 0; i < level.bottomLayers().size(); ++i) {
        items.emplace_back(i, visibleCb);
        bottomBox->pack(items.back().row, true, false);
    }
    for (unsigned int i = level.bottomLayers().size();
         i < level.bottomLayers().size() + level.ysortLayers().size();
         ++i) {
        items.emplace_back(i, visibleCb);
        ysortBox->pack(items.back().row, true, false);
    }
    for (unsigned int i = level.bottomLayers().size() + level.ysortLayers().size();
         i < level.layerCount();
         ++i) {
        items.emplace_back(i, visibleCb);
        topBox->pack(items.back().row, true, false);
    }

    page->pack(box, true, true);
}

Layers::LayerRow::LayerRow(unsigned int i, const VisibleCb& visibleCb)
: index(i) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal));

    name = Label::create("Layer " + std::to_string(i));
    name->setColor(sf::Color(bl::util::Random::get<std::uint8_t>(0, 255),
                             bl::util::Random::get<std::uint8_t>(0, 255),
                             bl::util::Random::get<std::uint8_t>(0, 255)),
                   sf::Color::Transparent);
    row->pack(name, true, false);

    visibleToggle = CheckButton::create("Visible");
    visibleToggle->setValue(true);
    visibleToggle->getSignal(Action::ValueChanged)
        .willAlwaysCall([this, visibleCb](const Action&, Element*) {
            visibleCb(index, visibleToggle->getValue());
        });
    row->pack(visibleToggle, false, true);

    upBut = Button::create("Up");
    row->pack(upBut, false, true);

    downBut = Button::create("Down");
    row->pack(downBut, false, true);

    delBut = Button::create("Delete");
    delBut->setColor(sf::Color(180, 15, 15), sf::Color(60, 0, 0));
    row->pack(delBut, false, true);
}

} // namespace page
} // namespace editor
