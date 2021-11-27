#ifndef EDITOR_COMPONENTS_STATBOX_HPP
#define EDITOR_COMPONENTS_STATBOX_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/Stats.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Provides an inline GUI entry for stats in either EV mode or IV mode
 *
 * @ingroup Components
 *
 */
class StatBox {
public:
    /// Mode the entry is in
    enum Mode { EV, IV };

    /**
     * @brief Construct a new Stat Box component
     *
     * @param mode EV or IV mode
     */
    StatBox(Mode mode);

    /**
     * @brief Returns the current Stats value entered
     *
     */
    core::pplmn::Stats currentValue() const;

    /**
     * @brief Updates the GUI elements with the given value
     *
     * @param value The value to set
     */
    void update(const core::pplmn::Stats& value);

    /**
     * @brief Resets all stats to 0
     *
     */
    void reset();

    /**
     * @brief Packs the GUI elements into the given container
     *
     * @param container The container to pack into
     */
    void pack(bl::gui::Box& container);

private:
    const Mode mode;
    bl::gui::TextEntry::Ptr hpEntry;
    bl::gui::TextEntry::Ptr atkEntry;
    bl::gui::TextEntry::Ptr defEntry;
    bl::gui::TextEntry::Ptr spAtkEntry;
    bl::gui::TextEntry::Ptr spDefEntry;
    bl::gui::TextEntry::Ptr spdEntry;

    void onChange(bl::gui::Element* e);
};

} // namespace component
} // namespace editor

#endif