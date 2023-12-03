#ifndef EDITOR_COMPONENTS_GAMETESTINGWINDOW_HPP
#define EDITOR_COMPONENTS_GAMETESTINGWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/GameSave.hpp>
#include <Editor/Components/ItemSelector.hpp>
#include <Editor/Components/PeoplemonSelector.hpp>

namespace editor
{
namespace component
{
class GameTestingWindow {
public:
    using ActionCb = std::function<void()>;

    GameTestingWindow(const ActionCb& onSave);

    void open(bl::gui::GUI* gui, core::file::GameSave& save);

private:
    core::file::GameSave* activeSave;
    const ActionCb onSave;

    bl::gui::GUI::Ptr gui;
    bl::gui::Window::Ptr window;

    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr moneyEntry;

    bl::gui::Slider::Ptr timeSlider;
    bl::gui::Label::Ptr timeLabel;

    bl::gui::SelectBox::Ptr itemBox;
    component::ItemSelector::Ptr itemSelect;

    bl::gui::TextEntry::Ptr flagEntry;
    bl::gui::SelectBox::Ptr flagBox;

    bl::gui::SelectBox::Ptr pplBox;
    component::PeoplemonSelector::Ptr pplSelect;
    bl::gui::ComboBox::Ptr levelSelect;

    void updateTimeLabel(const core::system::Clock::Time& time);

    void addItem();
    void rmItem();

    void addFlag();
    void rmFlag();

    void addPeoplemon();
    void rmPeoplemon();

    void doSave();
    void cancel();
};

} // namespace component
} // namespace editor

#endif
