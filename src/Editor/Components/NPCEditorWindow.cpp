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
using FileUtil = bl::util::FileUtil;

NpcEditorWindow::NpcEditorWindow(const SelectCb& cb, const CloseCb& ccb)
: selectCb(cb)
, closeCb(ccb)
, clean(true)
, filePicker(core::Properties::NpcPath(), {"npc"},
             std::bind(&NpcEditorWindow::onChooseFile, this, std::placeholders::_1),
             [this]() {
                 filePicker.close();
                 window->setForceFocus(true);
             })
, animWindow(true, std::bind(&NpcEditorWindow::onChooseAnimation, this, std::placeholders::_1),
             std::bind(&NpcEditorWindow::forceWindowOnTop, this))
, behaviorEditor(
      std::bind(&NpcEditorWindow::makeDirty, this), [this]() { window->setForceFocus(false); },
      std::bind(&NpcEditorWindow::forceWindowOnTop, this))
, conversationWindow(std::bind(&NpcEditorWindow::onChooseConversation, this, std::placeholders::_1),
                     std::bind(&NpcEditorWindow::forceWindowOnTop, this)) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 8), "NPC Editor");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) { hide(); });

    Box::Ptr row       = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr newBut = Button::create("New");
    newBut->setTooltip("Create a new NPC file");
    newBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (confirmDiscard()) {
            makingNew = true;
            window->setForceFocus(false);
            filePicker.open(FilePicker::CreateNew, "New NPC", parent, true);
        }
    });
    Button::Ptr setBut = Button::create("Set File");
    setBut->setTooltip("Change the file to save this NPC to");
    setBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (fileLabel->getText() == EmptyFile || confirmDiscard()) {
            makingNew = true;
            window->setForceFocus(false);
            filePicker.open(FilePicker::CreateOrPick, "Set NPC File", parent, false);
        }
    });
    Button::Ptr openBut = Button::create("Open");
    openBut->setTooltip("Open a different NPC file");
    openBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (confirmDiscard()) {
            makingNew = false;
            window->setForceFocus(false);
            filePicker.open(FilePicker::PickExisting, "Open NPC", parent, false);
        }
    });
    saveBut = Button::create("Save");
    saveBut->setTooltip("Overwrite the current NPC file");
    saveBut->setColor(sf::Color::Yellow, sf::Color::Blue);
    saveBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (validate(true)) {
            core::file::NPC npc;
            npc.name()         = nameEntry->getInput();
            npc.conversation() = convLabel->getText();
            npc.animation()    = animLabel->getText();
            npc.behavior()     = behaviorEditor.getValue();
            if (!npc.save(bl::util::FileUtil::joinPath(core::Properties::NpcPath(),
                                                       fileLabel->getText()))) {
                bl::dialog::tinyfd_messageBox("Error", "Failed to save NPC", "ok", "error", 1);
            }
            else {
                makeClean();
            }
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
    nameEntry->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&NpcEditorWindow::makeDirty, this));
    row->pack(label, false, true);
    row->pack(nameEntry, true, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr animBut = Button::create("Select Anim");
    animBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        animWindow.open(parent, core::Properties::CharacterAnimationPath(), animLabel->getText());
    });
    animLabel = Label::create("animation.anim");
    animLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(animBut, false, true);
    row->pack(animLabel, true, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr convBut = Button::create("Select Conversation");
    convBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        conversationWindow.open(parent, convLabel->getText());
    });
    convLabel = Label::create("conversation.conv");
    convLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(convBut, false, true);
    row->pack(convLabel, true, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    behaviorEditor.pack(row);
    window->pack(row, true, false);

    row                   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr selectBut = Button::create("Use NPC");
    selectBut->setTooltip("Use the current NPC file");
    selectBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (confirmDiscard()) {
            if (validate(false)) {
                selectCb(fileLabel->getText());
                hide();
            }
        }
    });
    selectBut->setColor(sf::Color::Blue, sf::Color::Black);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        hide();
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
    window->setForceFocus(true);
}

void NpcEditorWindow::onChooseFile(const std::string& file) {
    const std::string f =
        bl::util::FileUtil::getExtension(file) == core::Properties::NpcFileExtension() ?
            file :
            file + "." + core::Properties::NpcFileExtension();
    filePicker.close();
    window->setForceFocus(true);
    if (makingNew) { makeDirty(); }
    else {
        reset();
        load(f);
        makeClean();
    }
    fileLabel->setText(f);
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
    behaviorEditor.configure(parent, {});
}

void NpcEditorWindow::load(const std::string& file) {
    core::file::NPC npc;
    if (npc.load(FileUtil::joinPath(core::Properties::NpcPath(), file))) {
        nameEntry->setInput(npc.name());
        animLabel->setText(npc.animation());
        convLabel->setText(npc.conversation());
        behaviorEditor.configure(parent, npc.behavior());
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
    const std::string p =
        FileUtil::joinPath(core::Properties::ConversationPath(), convLabel->getText());
    if (!FileUtil::exists(p)) {
        bl::dialog::tinyfd_messageBox("Warning", "Bad conversation", "ok", "warning", 1);
        return false;
    }
    if (behaviorEditor.getValue().type() == core::file::Behavior::Type::FollowingPath &&
        behaviorEditor.getValue().path().paces.empty()) {
        bl::dialog::tinyfd_messageBox("Warning", "Behavior path is empty", "ok", "warning", 1);
        return false;
    }
    return true;
}

bool NpcEditorWindow::confirmDiscard() const {
    if (!clean) {
        return 1 == bl::dialog::tinyfd_messageBox(
                        "Warning", "Discard unsaved changes?", "yesno", "warning", 0);
    }
    return true;
}

void NpcEditorWindow::hide() {
    window->remove();
    filePicker.close();
    animWindow.hide();
    behaviorEditor.hide();
    window->setForceFocus(false);
    closeCb();
}

void NpcEditorWindow::onChooseAnimation(const std::string& f) {
    animLabel->setText(f);
    makeDirty();
}

void NpcEditorWindow::onChooseConversation(const std::string& c) {
    convLabel->setText(c);
    window->setForceFocus(true);
    makeDirty();
}

void NpcEditorWindow::forceWindowOnTop() { window->setForceFocus(true); }

} // namespace component
} // namespace editor
