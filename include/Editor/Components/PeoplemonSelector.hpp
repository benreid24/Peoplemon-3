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
     */
    static Ptr create();

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
    PeoplemonSelector();
};

} // namespace component
} // namespace editor

#endif
