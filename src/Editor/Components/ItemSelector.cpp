#include <Editor/Components/ItemSelector.hpp>

namespace editor
{
namespace component
{
std::vector<core::item::Id> ItemSelector::idLookup;

ItemSelector::Ptr ItemSelector::create(const ChangeCb& cb) { return Ptr(new ItemSelector(cb)); }

ItemSelector::ItemSelector(const ChangeCb& cb)
: ComboBox() {
    refresh();
    getSignal(bl::gui::Event::ValueChanged)
        .willAlwaysCall([this, cb](const bl::gui::Event&, bl::gui::Element*) {
            if (cb) cb(currentItem());
        });
    setMaxHeight(300.f);
}

core::item::Id ItemSelector::currentItem() const {
    return getSelectedOption() >= 0 && !idLookup.empty() ? idLookup[getSelectedOption()] :
                                                           core::item::Id::Unknown;
}

void ItemSelector::setItem(core::item::Id item) {
    setSelectedOption(core::item::Item::getName(item), false);
}

void ItemSelector::refresh() {
    const core::item::Id ci = currentItem();

    idLookup.clear();
    idLookup.reserve(core::item::Item::validIds().size());
    clearOptions();

    int i = 0;
    for (const core::item::Id item : core::item::Item::validIds()) {
        idLookup.push_back(item);
        addOption(core::item::Item::getName(item));
        if (item == ci) { setSelectedOption(i, false); }
        ++i;
    }

    if (ci == core::item::Id::Unknown) setSelectedOption(0, false);
}

} // namespace component
} // namespace editor
