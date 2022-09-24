#include <Editor/Components/TypeSelector.hpp>

namespace editor
{
namespace component
{
namespace
{
constexpr core::pplmn::Type indexToType[] = {core::pplmn::Type::None,
                                             core::pplmn::Type::Normal,
                                             core::pplmn::Type::Intelligent,
                                             core::pplmn::Type::Funny,
                                             core::pplmn::Type::Athletic,
                                             core::pplmn::Type::Quiet,
                                             core::pplmn::Type::Awkward,
                                             core::pplmn::Type::PartyAnimal};
constexpr std::size_t typeCount           = sizeof(indexToType) / sizeof(indexToType[0]);
} // namespace

TypeSelector::Ptr TypeSelector::create() { return Ptr{new TypeSelector()}; }

TypeSelector::TypeSelector()
: ComboBox() {
    addOption("None");
    addOption("Normal");
    addOption("Intelligent");
    addOption("Funny");
    addOption("Athletic");
    addOption("Quiet");
    addOption("Awkward");
    addOption("PartyAnimal");

    setSelectedOption(0);
}

core::pplmn::Type TypeSelector::currentType() const { return indexToType[getSelectedOption()]; }

void TypeSelector::setCurrentType(core::pplmn::Type type) {
    for (std::size_t i = 0; i < typeCount; ++i) {
        if (type == indexToType[i]) {
            setSelectedOption(i);
            return;
        }
    }
    BL_LOG_WARN << "Received bad peoplemon type: " << type;
    setSelectedOption(0);
}

} // namespace component
} // namespace editor
