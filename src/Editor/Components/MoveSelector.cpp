#include <Editor/Components/MoveSelector.hpp>

#include <Core/Peoplemon/Move.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

std::vector<core::pplmn::MoveId> MoveSelector::idLookup;

MoveSelector::Ptr MoveSelector::create(const ChangeCb& ccb) { return Ptr(new MoveSelector(ccb)); }

MoveSelector::MoveSelector(const ChangeCb& ccb)
: ComboBox() {
    refresh();
    getSignal(Event::ValueChanged).willAlwaysCall([this, ccb](const Event&, Element*) {
        if (ccb) (ccb(currentMove()));
    });
    setMaxHeight(300.f);
}

core::pplmn::MoveId MoveSelector::currentMove() const {
    return getSelectedOption() >= 0 && !idLookup.empty() ? idLookup[getSelectedOption()] :
                                                           core::pplmn::MoveId::Unknown;
}

void MoveSelector::setCurrentMove(core::pplmn::MoveId m) {
    setSelectedOption(core::pplmn::Move::name(m), false);
}

void MoveSelector::refresh() {
    const core::pplmn::MoveId m = currentMove();

    idLookup.clear();
    idLookup.reserve(core::pplmn::Move::validIds().size());
    clearOptions();

    int i = 0;
    for (const core::pplmn::MoveId move : core::pplmn::Move::validIds()) {
        idLookup.push_back(move);
        addOption(core::pplmn::Move::name(move));
        if (m == move) { setSelectedOption(i, false); }
        ++i;
    }

    if (m == core::pplmn::MoveId::Unknown) setSelectedOption(0, false);
}

} // namespace component
} // namespace editor
