#include <Editor/Components/OwnedPeoplemonWindow.hpp>

#include <Core/Peoplemon/Peoplemon.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

OwnedPeoplemonWindow::OwnedPeoplemonWindow(const NotifyCB& fcb, const NotifyCB& ccb)
: onFinish(fcb)
, onCancel(ccb) {
    window =
        Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Owned Peoplemon Editor");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) {
        hide();
        onCancel();
    });

    idSelect = ComboBox::create();
    idSelect->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event& e, Element*) {
        const auto it = idMap.find(static_cast<int>(e.inputValue()));
        if (it != idMap.end() && it->second != core::pplmn::Id::Unknown) { syncMoves(it->second); }
    });

    // TODO - create other gui elements
}

void OwnedPeoplemonWindow::show(const GUI::Ptr& p, const core::pplmn::OwnedPeoplemon& value) {
    parent = p;

    idSelect->clearOptions();
    const std::vector<core::pplmn::Id>& validIds = core::pplmn::Peoplemon::validIds();
    for (unsigned int i = 0; i < validIds.size(); ++i) {
        idSelect->addOption(std::to_string(static_cast<unsigned int>(validIds[i])) + ": " +
                            core::pplmn::Peoplemon::name(validIds[i]));
        idMap[i] = validIds[i];
    }
    idMap[-1] = core::pplmn::Id::Unknown;

    // TODO - sync values in other gui elements
}

} // namespace component
} // namespace editor
