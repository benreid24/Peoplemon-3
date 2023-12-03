#ifndef EDITOR_COMPONENTS_CATCHREGIONWINDOW_HPP
#define EDITOR_COMPONENTS_CATCHREGIONWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/CatchRegion.hpp>
#include <Editor/Components/PeoplemonSelector.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Window that opens to edit a catch region in maps
 *
 * @ingroup Components
 *
 */
class CatchRegionWindow {
public:
    /// Called when the region is saved
    using TriggerCb = std::function<void()>;

    /**
     * @brief Construct a new Catch Region Window
     *
     * @param onSave Callback for when the region should be saved
     */
    CatchRegionWindow(const TriggerCb& onSave);

    /**
     * @brief Opens the window with the given region value
     *
     * @param gui The main gui object
     * @param region The region to populate with
     */
    void open(bl::gui::GUI* gui, const core::map::CatchRegion& region);

    /**
     * @brief Returns the current region value as inputed by the user
     *
     */
    const core::map::CatchRegion& getValue();

private:
    const TriggerCb onSave;
    core::map::CatchRegion value;

    struct Row {
        using Callback = std::function<void(bl::gui::Element*)>;

        bl::gui::Box::Ptr row;
        PeoplemonSelector::Ptr idSelect;
        bl::gui::TextEntry::Ptr minLevel;
        bl::gui::TextEntry::Ptr maxLevel;
        bl::gui::Slider::Ptr freqSlider;
        bl::gui::Label::Ptr freqLabel;
        bl::gui::CheckButton::Ptr lockBut;
        bl::gui::Button::Ptr delBut;

        Row(const Callback& freqChange, const Callback& del);
    };

    bl::gui::Window::Ptr window;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::ScrollArea::Ptr peopleScroll;
    std::vector<Row> rows;

    void close();
    void save();
    void add();
    void remove(bl::gui::Element* e);
    void freqChange(bl::gui::Element* e);
    bool validate() const;
};

} // namespace component
} // namespace editor

#endif
