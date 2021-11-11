#ifndef EDITOR_COMPONENTS_SCRIPTSELECTOR_HPP
#define EDITOR_COMPONENTS_SCRIPTSELECTOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <functional>

namespace editor
{
namespace component
{
/**
 * @brief GUI component for selecting or writing scripts. Performs syntax validation as well
 *
 * @ingroup Components
 *
 */
class ScriptSelector {
public:
    /// Callback type for when a script is chosen
    using OnSelect = std::function<void(const std::string&)>;

    /// Called when the window is closed without a selection
    using OnCancel = std::function<void()>;

    /**
     * @brief Construct a new Script Selector dialog
     *
     * @param onSelect Callback to call when a script is chosen
     * @param onCancel Called when the window is closed without a selection
     */
    ScriptSelector(const OnSelect& onSelect, const OnCancel& onCancel);

    /**
     * @brief Opens the dialog to select a script
     *
     * @param parent The parent to add the GUI elements to
     * @param value The default script value to fill
     */
    void open(const bl::gui::GUI::Ptr& parent, const std::string& value = {});

private:
    const OnSelect onSelect;
    const OnCancel onCancel;
    bl::gui::Window::Ptr window;
    bl::gui::TextEntry::Ptr scriptInput;
    bl::gui::Label::Ptr errorLabel;
    bool error;

    bl::gui::FilePicker picker;
    bl::gui::GUI::Ptr parent;
    void onPick(const std::string& s);

    void checkSyntax();
};

} // namespace component
} // namespace editor

#endif
