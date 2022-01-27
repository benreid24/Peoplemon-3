#ifndef EDITOR_COMPONENTS_PLAYLISTEDITORWINDOW_HPP
#define EDITOR_COMPONENTS_PLAYLISTEDITORWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace component
{
/**
 * @brief The playlist editor and chooser
 * 
 * @ingroup Components
 * 
 */
class PlaylistEditorWindow {
public:
    using SelectedCb = std::function<void(const std::string&)>;
    using CancelCb = std::function<void()>;

    /**
     * @brief Creates the playlist editor
     * 
     * @param onSelect Called when a playlist is selected
     * @param onCancel Called when the window is closed with no selection
     */
    PlaylistEditorWindow(const SelectedCb& onSelect, const CancelCb& onCancel);

    /**
     * @brief Opens the window with an optional file to load
     * 
     * @param gui The main gui object
     * @param plist The playlist to load
     */
    void open(const bl::gui::GUI::Ptr& gui, const std::string& plist);

private:
    bl::gui::GUI::Ptr gui;
    bl::gui::Window::Ptr window;
    bl::gui::SelectBox::Ptr songList;
    bl::gui::CheckButton::Ptr shuffleBut;
    bl::gui::CheckButton::Ptr loopShuffleBut;
    bl::gui::Label::Ptr fileLabel;
    bl::gui::Button::Ptr saveBut;

    bl::gui::FilePicker songPicker;
    void onSongPick(const std::string& song);

    bl::gui::FilePicker plistPicker;
    bool settingFile;
    void onPlaylistPick(const std::string& plist);

    void makeNew();
    void markDirty();
    void markClean();
    void removeSong();

    void save();
    void load(const std::string& file);
    void close();
    void closePickers();
};

}
}

#endif
