#include <Editor/Components/MoveSelector.hpp>

#include <BLIB/Util/Random.hpp>
#include <Core/Peoplemon/Move.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

MoveSelector::Ptr MoveSelector::create(bool ef, const ChangeCb& ccb) {
    return Ptr(new MoveSelector(ef, ccb));
}

MoveSelector::MoveSelector(bool ef, const ChangeCb& ccb)
: Box(LinePacker::create(LinePacker::Vertical, 8.f)) {
    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Filter:"), false, true);
    noMoveFilter = RadioButton::create("None", "none");
    noMoveFilter->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&MoveSelector::refresh, this));
    levelMoveFilter = RadioButton::create("Learned", "learn", noMoveFilter->getRadioGroup());
    levelMoveFilter->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&MoveSelector::refresh, this));
    poolMoveFilter = RadioButton::create("Pool", "pool", noMoveFilter->getRadioGroup());
    poolMoveFilter->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&MoveSelector::refresh, this));
    row->pack(noMoveFilter, false, true);
    row->pack(levelMoveFilter, false, true);
    row->pack(poolMoveFilter, false, true);

    if (ef) { pack(row, true, true); }

    selector = ComboBox::create();
    selector->getSignal(Event::ValueChanged).willAlwaysCall([this, ccb](const Event&, Element*) {
        if (ccb) (ccb(currentMove()));
    });
    selector->setMaxHeight(300.f);
    pack(selector, true, true);

    refresh();
    noMoveFilter->setValue(true);
}

core::pplmn::MoveId MoveSelector::currentMove() const {
    return selector->getSelectedOption() >= 0 ? validMoves[selector->getSelectedOption()] :
                                                core::pplmn::MoveId::Unknown;
}

void MoveSelector::setCurrentMove(core::pplmn::MoveId m) {
    selector->setSelectedOption(core::pplmn::Move::name(m), false);
}

void MoveSelector::notifyPeoplemon(core::pplmn::Id id, unsigned int l) {
    peoplemon = id;
    level     = l;
    refresh();
}

void MoveSelector::refresh() {
    const core::pplmn::MoveId m = currentMove();
    selector->clearOptions();

    if (noMoveFilter->getValue()) { validMoves = core::pplmn::Move::validIds(); }
    else if (levelMoveFilter->getValue()) {
        validMoves.clear();
        core::pplmn::Id ppl = peoplemon;
        while (ppl != core::pplmn::Id::Unknown) {
            for (unsigned int i = 0; i <= level; ++i) {
                const auto move = core::pplmn::Peoplemon::moveLearnedAtLevel(ppl, i);
                if (move != core::pplmn::MoveId::Unknown) {
                    if (std::find(validMoves.begin(), validMoves.end(), move) == validMoves.end()) {
                        validMoves.push_back(move);
                    }
                }
            }
            ppl = core::pplmn::Peoplemon::evolvesFrom(ppl);
        }
    }
    else {
        validMoves.clear();
        core::pplmn::Id ppl = peoplemon;
        while (ppl != core::pplmn::Id::Unknown) {
            for (const auto move : core::pplmn::Move::validIds()) {
                if (core::pplmn::Peoplemon::canLearnMove(ppl, move)) {
                    if (std::find(validMoves.begin(), validMoves.end(), move) == validMoves.end()) {
                        validMoves.push_back(move);
                    }
                }
            }
            ppl = core::pplmn::Peoplemon::evolvesFrom(ppl);
        }
    }

    int i = 0;
    for (const auto move : validMoves) {
        selector->addOption(core::pplmn::Move::name(move));
        ++i;
    }

    if (m == core::pplmn::MoveId::Unknown) selector->setSelectedOption(0, false);
}

void MoveSelector::selectRandom() {
    selector->setSelectedOption(bl::util::Random::get<int>(0, selector->optionCount() - 1));
}

} // namespace component
} // namespace editor
