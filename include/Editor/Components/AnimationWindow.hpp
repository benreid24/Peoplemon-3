#ifndef EDITOR_COMPONENTS_ANIMATIONWINDOW_HPP
#define EDITOR_COMPONENTS_ANIMATIONWINDOW_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <BLIB/Media.hpp>
#include <functional>

namespace editor
{
namespace component
{
/**
 * @brief A basic window to choose and view animations
 *
 * @ingroup Components
 *
 */
class AnimationWindow {
public:
    /// Called when an animation is chosen
    using ChooseCb = std::function<void(const std::string&)>;

    /**
     * @brief Construct a new Animation Window
     *
     * @param characterMode True to select folders of animations, false for single anims
     * @param chooseCb Callback to call when an animation is chosen
     */
    AnimationWindow(bool characterMode, const ChooseCb& chooseCb);

    /**
     * @brief Opens the animation picker window
     *
     * @param parent Parent GUI object
     * @param path Path to search for animations within
     * @param file Existing animation to use
     */
    void open(const bl::gui::GUI::Ptr& parent, const std::string& path, const std::string& file);

    /**
     * @brief Removes the window from view
     *
     */
    void hide();

private:
    const bool characterMode;
    const ChooseCb chooseCb;
    std::string path;
    bl::gui::Window::Ptr window;
    bl::gui::GUI::Ptr parent;
    bl::gui::Label::Ptr fileLabel;
    bl::gui::Animation::Ptr animation;
    bl::resource::Resource<bl::gfx::AnimationData>::Ref animSrc;
    bl::container::ObjectWrapper<bl::gui::FilePicker> filePicker;

    void packAnim(const std::string& f);
};

} // namespace component
} // namespace editor

#endif
