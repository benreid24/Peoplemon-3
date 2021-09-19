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
class AnimationWindow {
public:
    using ChooseCb = std::function<void(const std::string&)>;

    AnimationWindow(const ChooseCb& chooseCb);

    void open(const bl::gui::GUI::Ptr& parent, const std::string& path, const std::string& file);

private:
    const ChooseCb chooseCb;
    bl::gui::GUI::Ptr parent;
    std::string path;
    bl::gui::Window::Ptr window;
    bl::gui::Label::Ptr fileLabel;
    bl::gui::Box::Ptr animBox;
    bl::resource::Resource<bl::gfx::AnimationData>::Ref animSrc;
    bl::container::ObjectWrapper<bl::gui::FilePicker> filePicker;
};

} // namespace component
} // namespace editor

#endif
