#include <Editor/Components/NPCEditorWindow.hpp>

#include <Core/Files/NPC.hpp>
#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
namespace
{
const std::string EmptyFile = "<no file selected>";
}
using namespace bl::gui;
using FileUtil = bl::file::Util;

NpcEditorWindow::NpcEditorWindow(const SelectCb& cb)
: selectCb(cb)
, clean(true)
, filePicker(core::Properties::NpcPath(), {"npc"},
             std::bind(&NpcEditorWindow::onChooseFile, this, std::placeholders::_1),
             [this]() { filePicker.close(); }) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 8), "NPC Editor");
    window->getSignal(Action::Closed).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });

    Box::Ptr row       = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr newBut = Button::create("New");
    newBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (confirmDiscard()) {
            makingNew = true;
            reset();
            makeClean();
            filePicker.open(FilePicker::CreateNew, "New NPC", parent, true);
        }
    });
    Button::Ptr setBut = Button::create("Set File");
    setBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (fileLabel->getText() == EmptyFile || confirmDiscard()) {
            makingNew = true;
            filePicker.open(FilePicker::PickExisting, "New NPC", parent, false);
        }
    });
    Button::Ptr openBut = Button::create("Open");
    openBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (confirmDiscard()) {
            makingNew = false;
            filePicker.open(FilePicker::PickExisting, "Open NPC", parent, false);
        }
    });
    saveBut = Button::create("Save");
    saveBut->setColor(sf::Color::Yellow, sf::Color::Blue);
    saveBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (validate(true)) {
            core::file::NPC npc;
            npc.name() = nameEntry->getInput();
        }
    });
    fileLabel = Label::create("filename.npc");
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(newBut, false, true);
    row->pack(setBut, false, true);
    row->pack(openBut, false, true);
    row->pack(saveBut, false, true);
    row->pack(fileLabel, true, true);
    window->pack(row, true, false);

    row              = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Label::Ptr label = Label::create("Name:");
    nameEntry        = TextEntry::create();
    row->pack(label, false, true);
    row->pack(nameEntry, true, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr animBut = Button::create("Select Anim");
    animBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    animLabel = Label::create("animation.anim");
    animLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(animBut, false, true);
    row->pack(animLabel, true, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr convBut = Button::create("Select Conversation");
    convBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    convLabel = Label::create("conversation.conv");
    convLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(convBut, false, true);
    row->pack(convLabel, true, true);
    window->pack(row, true, false);

    // TODO - behavior editor

    row                   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr selectBut = Button::create("Use NPC");
    selectBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (confirmDiscard()) {
            if (validate(false)) {
                selectCb(fileLabel->getText());
                window->remove();
            }
        }
    });
    selectBut->setColor(sf::Color::Blue, sf::Color::Black);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(selectBut, false, true);
    row->pack(cancelBut, false, true);
    window->pack(row, true, false);
}

void NpcEditorWindow::show(GUI::Ptr p, const std::string& file) {
    parent = p;
    reset();
    if (!file.empty()) {
        fileLabel->setText(file);
        load(file);
    }
    makeClean();
    parent->pack(window);
}

void NpcEditorWindow::onChooseFile(const std::string& file) {
    filePicker.close();
    fileLabel->setText(file);
    if (makingNew) { makeDirty(); }
    else {
        load(file);
        makeClean();
    }
}

void NpcEditorWindow::makeClean() {
    saveBut->setColor(sf::Color::Green, sf::Color::Blue);
    clean = true;
}

void NpcEditorWindow::makeDirty() {
    saveBut->setColor(sf::Color::Yellow, sf::Color::Blue);
    clean = false;
}

void NpcEditorWindow::reset() {
    fileLabel->setText(EmptyFile);
    nameEntry->setInput("");
    animLabel->setText("<no anim selected>");
    convLabel->setText("<no conv selected>");
}

void NpcEditorWindow::load(const std::string& file) {
    core::file::NPC npc;
    if (npc.load(FileUtil::joinPath(core::Properties::NpcPath(), file))) {
        nameEntry->setInput(npc.name());
        animLabel->setText(npc.animation());
        convLabel->setText(npc.conversation());
    }
    else {
        bl::dialog::tinyfd_messageBox(
            "Error", std::string("Failed to load NPC:\n" + file).c_str(), "ok", "error", 1);
    }
}

bool NpcEditorWindow::validate(bool saving) const {
    if (saving) {
        if (fileLabel->getText() == EmptyFile) {
            bl::dialog::tinyfd_messageBox("Warning", "Please select a file", "ok", "warning", 1);
            return false;
        }
    }
    else {
        if (!FileUtil::exists(
                FileUtil::joinPath(core::Properties::NpcPath(), fileLabel->getText()))) {
            bl::dialog::tinyfd_messageBox("Warning", "Bad file selected", "ok", "warning", 1);
            return false;
        }
    }
    if (nameEntry->getInput().empty()) {
        bl::dialog::tinyfd_messageBox("Warning", "Enter a name", "ok", "warning", 1);
        return false;
    }
    if (!FileUtil::directoryExists(
            FileUtil::joinPath(core::Properties::CharacterAnimationPath(), animLabel->getText()))) {
        bl::dialog::tinyfd_messageBox("Warning", "Bad animation", "ok", "warning", 1);
        return false;
    }
    if (!FileUtil::exists(
            FileUtil::joinPath(core::Properties::ConversationPath(), animLabel->getText()))) {
        bl::dialog::tinyfd_messageBox("Warning", "Bad conversation", "ok", "warning", 1);
        return false;
    }
    // TODO - validate behavior
    return true;
}

bool NpcEditorWindow::confirmDiscard() const {
    if (!clean) {
        return 1 == bl::dialog::tinyfd_messageBox(
                        "Warning", "Discard unsaved changes?", "yesno", "warning", 0);
    }
    return true;
}

} // namespace component
} // namespace editor
