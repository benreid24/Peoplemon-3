#ifndef EDITOR_COMPONENTS_HELPERS_BEHAVIOREDITOR_HPP
#define EDITOR_COMPONENTS_HELPERS_BEHAVIOREDITOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/Behavior.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Inline GUI elements with openable window for editing behaviors
 *
 * @ingroup Components
 *
 */
class BehaviorEditor {
public:
    using OnSetCb = std::function<void()>;

    /**
     * @brief Construct a new Behavior Editor
     *
     * @param cb Callback for when the behavior is modified
     *
     */
    BehaviorEditor(const OnSetCb& cb);

    /**
     * @brief Returns the behavior value
     *
     */
    const core::file::Behavior& getValue() const;

    /**
     * @brief Packs the GUI elements for the editor into the given container
     *
     * @param parent The box to pack in
     */
    void pack(bl::gui::Box::Ptr parent);

    /**
     * @brief Sets the parent GUI object and current value. Call before using
     *
     * @param parent The parent GUI object
     * @param behavior The behavior value to init with
     */
    void configure(bl::gui::GUI::Ptr parent, const core::file::Behavior& behavior);

    /**
     * @brief Hides the editor window if opened
     *
     */
    void hide();

private:
    class PathEditor {
    public:
        PathEditor();
        void pack(bl::gui::Box::Ptr box);
        void init(const core::file::Behavior::Path& path);
        const core::file::Behavior::Path& getValue() const;

    private:
        core::file::Behavior::Path value;
        bl::gui::ScrollArea::Ptr container;
        bl::gui::CheckButton::Ptr toggle;

        void sync();
    };

    const OnSetCb onSetCb;
    core::file::Behavior value;
    core::file::Behavior ogValue;
    bl::gui::GUI::Ptr parent;
    bl::gui::Window::Ptr window;
    bl::gui::Notebook::Ptr notebook;
    bl::gui::Label::Ptr typeLabel;
    bl::gui::ComboBox::Ptr dirBox;
    bl::gui::ComboBox::Ptr spinBox;
    bl::gui::TextEntry::Ptr radiusEntry;
    PathEditor pathEditor;
};

} // namespace component
} // namespace editor

#endif
