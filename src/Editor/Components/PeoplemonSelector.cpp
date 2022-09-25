#include <Core/Peoplemon/Peoplemon.hpp>
#include <Editor/Components/PeoplemonSelector.hpp>

namespace editor
{
namespace component
{
PeoplemonSelector::Ptr PeoplemonSelector::create(bool au) { return Ptr(new PeoplemonSelector(au)); }

PeoplemonSelector::PeoplemonSelector(bool au)
: ComboBox()
, allowUnknown(au) {
    refresh();
}

core::pplmn::Id PeoplemonSelector::currentPeoplemon() const {
    return getSelectedOption() < 0 ? core::pplmn::Id::Unknown :
                                     core::pplmn::Peoplemon::validIds()[getSelectedOption()];
}

void PeoplemonSelector::setPeoplemon(core::pplmn::Id id) {
    if (allowUnknown && id == core::pplmn::Id::Unknown) {
        setSelectedOption(0);
        return;
    }

    for (unsigned int i = 0; i < core::pplmn::Peoplemon::validIds().size(); ++i) {
        if (core::pplmn::Peoplemon::validIds()[i] == id) {
            setSelectedOption(allowUnknown ? i + 1 : i);
            return;
        }
    }

    BL_LOG_WARN << "Tried to set invalid Peoplemon id: " << id;
}

void PeoplemonSelector::refresh() {
    clearOptions();
    if (allowUnknown) { addOption("Unknown/None"); }
    for (const core::pplmn::Id id : core::pplmn::Peoplemon::validIds()) {
        addOption(std::to_string(static_cast<int>(id)) + ": " + core::pplmn::Peoplemon::name(id));
    }
}

} // namespace component
} // namespace editor
