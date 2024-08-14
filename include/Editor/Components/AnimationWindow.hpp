#ifndef EDITOR_COMPONENTS_ANIMATIONWINDOW_HPP
#define EDITOR_COMPONENTS_ANIMATIONWINDOW_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <functional>
#include <optional>

namespace editor
{
namespace component
{
/**
 * @brief A basic window to choose and view animations
 *
 * @ingroup UIComponents
 */
class AnimationWindow {
public:
    /// Called when an animation is chosen
    using ChooseCb = std::function<void(const std::string&)>;

    /// Called when the window is closed
    using CloseCb = std::function<void()>;

    /**
     * @brief Construct a new Animation Window
     *
     * @param characterMode True to select folders of animations, false for single anims
     * @param chooseCb Callback to call when an animation is chosen
     * @param closeCb Callback for when the window is closed
     */
    AnimationWindow(bool characterMode, const ChooseCb& chooseCb, const CloseCb& closeCb);

    /**
     * @brief Opens the animation picker window
     *
     * @param parent Parent GUI object
     * @param path Path to search for animations within
     * @param file Existing animation to use
     */
    void open(bl::gui::GUI* parent, const std::string& path, const std::string& file);

    /**
     * @brief Removes the window from view
     */
    void hide();

private:
    const bool characterMode;
    const ChooseCb chooseCb;
    const CloseCb closeCb;
    std::string path;
    bl::gui::Window::Ptr window;
    bl::gui::GUI* parent;
    bl::gui::Label::Ptr fileLabel;
    bl::gui::Animation::Ptr animation;
    bl::resource::Ref<bl::gfx::a2d::AnimationData> animSrc;
    std::optional<bl::gui::FilePicker> filePicker;

    void packAnim(const std::string& f);
};

} // namespace component
} // namespace editor

#endif
