#include <Editor/Components/PlaylistEditorWindow.hpp>

#include <BLIB/Audio.hpp>
#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

PlaylistEditorWindow::PlaylistEditorWindow(const SelectedCb& oscb, const CancelCb& occb)
: onSelect(oscb)
, onCancel(occb)
, songPicker(core::Properties::MusicPath(), {"ogg", "wav"},
             std::bind(&PlaylistEditorWindow::onSongPick, this, std::placeholders::_1),
             std::bind(&PlaylistEditorWindow::closePickers, this))
, plistPicker(core::Properties::PlaylistPath(), {"plst"},
              std::bind(&PlaylistEditorWindow::onPlaylistPick, this, std::placeholders::_1),
              std::bind(&PlaylistEditorWindow::closePickers, this)) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4.f), "Playlist Editor");
    window->getSignal(Event::Closed).willAlwaysCall(std::bind(&PlaylistEditorWindow::close, this));

    Box::Ptr row    = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    Button::Ptr but = Button::create("New");
    but->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PlaylistEditorWindow::makeNew, this));
    row->pack(but, false, true);
    but = Button::create("Open");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        settingFile = false;
        window->setForceFocus(false);
        plistPicker.open(FilePicker::PickExisting, "Open Playlist", gui);
    });
    row->pack(but, false, true);
    but = Button::create("Set file");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        settingFile = true;
        window->setForceFocus(false);
        plistPicker.open(FilePicker::CreateOrPick, "Set Playlist File", gui);
    });
    row->pack(but, false, true);
    saveBut = Button::create("Save");
    saveBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PlaylistEditorWindow::save, this));
    row->pack(saveBut, false, true);
    fileLabel = Label::create("<set a file>");
    fileLabel->setColor(sf::Color(20, 230, 245), sf::Color::Transparent);
    row->pack(fileLabel, true, true);
    window->pack(row);

    row      = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    songList = SelectBox::create();
    songList->setRequisition({400.f, 350.f});
    songList->setMaxSize({400.f, 400.f});
    row->pack(songList, true, true);

    Box::Ptr column = Box::create(LinePacker::create(LinePacker::Vertical, 6.f));
    but             = Button::create("Add Song");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        songPicker.open(FilePicker::PickExisting, "Add Song", gui);
    });
    column->pack(but, true, false);

    shuffleBut = CheckButton::create("Shuffle");
    column->pack(shuffleBut);
    loopShuffleBut = CheckButton::create("Reshuffle on loop");
    column->pack(loopShuffleBut);

    but = Button::create("Remove");
    but->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PlaylistEditorWindow::removeSong, this));
    but->setColor(sf::Color(170, 0, 0), sf::Color::Black);
    column->pack(but);
    row->pack(column, false, true);
    window->pack(row, true, true);

    but = Button::create("Use Playlist");
    but->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&PlaylistEditorWindow::select, this));
    but->setColor(sf::Color(20, 230, 255), sf::Color::Black);
    window->pack(but);

    markClean();
}

void PlaylistEditorWindow::open(GUI* g, const std::string& p) {
    gui = g;
    if (!p.empty()) { load(p); }
    else { makeNew(); }
    gui->pack(window);
    window->setForceFocus(true);
}

void PlaylistEditorWindow::onSongPick(const std::string& song) {
    songPicker.close();
    window->setForceFocus(true);
    songList->addOption(song);
    markDirty();
}

void PlaylistEditorWindow::onPlaylistPick(const std::string& p) {
    const std::string plst = bl::util::FileUtil::getExtension(p) == "plst" ? p : p + ".plst";
    if (settingFile) {
        fileLabel->setText(plst);
        markDirty();
    }
    else { load(plst); }
    closePickers();
}

void PlaylistEditorWindow::makeNew() {
    if (!confirmUnsaved()) return;

    songList->clearOptions();
    fileLabel->setText("<set a file>");
    shuffleBut->setValue(true);
    loopShuffleBut->setValue(false);
    markClean();
}

void PlaylistEditorWindow::markDirty() {
    saveBut->setColor(sf::Color::Red, sf::Color::Black);
    dirty = true;
}

void PlaylistEditorWindow::markClean() {
    saveBut->setColor(sf::Color::Red, sf::Color::Black);
    dirty = false;
}

void PlaylistEditorWindow::removeSong() {
    const auto o = songList->getSelectedOption();
    if (o.has_value()) { songList->removeOption(o.value()); }
}

void PlaylistEditorWindow::save() {
    bl::audio::Playlist plst;
    plst.setShuffle(shuffleBut->getValue());
    plst.setShuffleOnLoop(loopShuffleBut->getValue());

    std::vector<std::string> songs;
    songList->getAllOptions(songs);
    for (const auto& song : songs) { plst.addSong(song); }

    if (plst.saveToFile(
            bl::util::FileUtil::joinPath(core::Properties::PlaylistPath(), fileLabel->getText()))) {
        markClean();
    }
    else { bl::dialog::tinyfd_messageBox("Error", "Failed to save playlist", "ok", "error", 1); }
}

void PlaylistEditorWindow::load(const std::string& file) {
    if (!confirmUnsaved()) return;

    bl::audio::Playlist plst;
    if (!plst.loadFromFile(bl::util::FileUtil::joinPath(core::Properties::PlaylistPath(), file))) {
        bl::dialog::tinyfd_messageBox("Error", "Failed to load playlist", "ok", "error", 1);
        return;
    }

    fileLabel->setText(file);
    songList->clearOptions();
    for (const auto& song : plst.getSongList()) { songList->addOption(song); }
    shuffleBut->setValue(plst.shuffling());
    loopShuffleBut->setValue(plst.shufflingOnLoop());
    markClean();
}

void PlaylistEditorWindow::close() {
    closePickers();
    window->setForceFocus(false);
    window->remove();
    onCancel();
}

void PlaylistEditorWindow::select() {
    if (!confirmUnsaved()) return;

    if (!bl::util::FileUtil::exists(
            bl::util::FileUtil::joinPath(core::Properties::PlaylistPath(), fileLabel->getText()))) {
        bl::dialog::tinyfd_messageBox("Error", "Please select a playlist", "ok", "error", 1);
        return;
    }

    std::vector<std::string> songs;
    songList->getAllOptions(songs);
    if (songs.empty()) {
        bl::dialog::tinyfd_messageBox(
            "Error", "Please add at least one song mate", "ok", "error", 1);
        return;
    }

    close();
    onSelect(fileLabel->getText());
}

void PlaylistEditorWindow::closePickers() {
    songPicker.close();
    plistPicker.close();
    window->setForceFocus(true);
}

bool PlaylistEditorWindow::confirmUnsaved() {
    if (dirty) {
        return 1 == bl::dialog::tinyfd_messageBox(
                        "Warning", "Discard unsaved changes", "yesno", "warning", 0);
    }
    return true;
}

} // namespace component
} // namespace editor
