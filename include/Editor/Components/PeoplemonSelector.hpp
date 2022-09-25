#ifndef EDITOR_COMPONENTS_PEOPLEMONSELECTOR_HPP
#define EDITOR_COMPONENTS_PEOPLEMONSELECTOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/Id.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Special combobox for Peoplemon ids
 *
 * @ingroup Components
 *
 */
class PeoplemonSelector : public bl::gui::ComboBox {
public:
    using Ptr = std::shared_ptr<PeoplemonSelector>;

    /**
     * @brief Creates a new peoplemon selector
     *
     * @param allowUnknown Allows Unknown/None option to be selected
     *
     */
    static Ptr create(bool allowUnknown = false);

    /**
     * @brief Returns the currently selected peoplemon
     *
     */
    core::pplmn::Id currentPeoplemon() const;

    /**
     * @brief Sets the currently selected Peoplemon
     *
     */
    void setPeoplemon(core::pplmn::Id id);

    /**
     * @brief Refreshes the list of peoplemon
     *
     */
    void refresh();

private:
    const bool allowUnknown;

    PeoplemonSelector(bool allowUnknown);
};

} // namespace component
} // namespace editor

#endif
