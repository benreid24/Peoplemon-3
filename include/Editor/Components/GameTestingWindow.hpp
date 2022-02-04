#ifndef EDITOR_COMPONENTS_GAMETESTINGWINDOW_HPP
#define EDITOR_COMPONENTS_GAMETESTINGWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace component
{
class GameTestingWindow {
public:
    GameTestingWindow();

    void open(const bl::gui::GUI::Ptr& gui);

private:
    bl::gui::GUI::Ptr gui;
    bl::gui::Window::Ptr window;

    // TODO - data and gui elements for save editing
};

} // namespace component
} // namespace editor

#endif
