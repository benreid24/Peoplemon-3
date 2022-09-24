#include <Core/Peoplemon/Peoplemon.hpp>
#include <Editor/Components/PeoplemonSelector.hpp>

namespace editor
{
namespace component
{
PeoplemonSelector::Ptr PeoplemonSelector::create() { return Ptr(new PeoplemonSelector()); }

PeoplemonSelector::PeoplemonSelector()
: ComboBox() {
    refresh();
}

core::pplmn::Id PeoplemonSelector::currentPeoplemon() const {
    return getSelectedOption() < 0 ? core::pplmn::Id::Unknown :
                                     core::pplmn::Peoplemon::validIds()[getSelectedOption()];
}

void PeoplemonSelector::setPeoplemon(core::pplmn::Id id) {
    for (unsigned int i = 0; i < core::pplmn::Peoplemon::validIds().size(); ++i) {
        if (core::pplmn::Peoplemon::validIds()[i] == id) {
            setSelectedOption(i);
            return;
        }
    }

    BL_LOG_WARN << "Tried to set invalid Peoplemon id: " << id;
}

void PeoplemonSelector::refresh() {
    clearOptions();
    for (const core::pplmn::Id id : core::pplmn::Peoplemon::validIds()) {
        addOption(std::to_string(static_cast<int>(id)) + ": " + core::pplmn::Peoplemon::name(id));
    }
}

} // namespace component
} // namespace editor
