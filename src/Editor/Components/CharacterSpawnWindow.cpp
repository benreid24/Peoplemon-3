#include <Editor/Components/CharacterSpawnWindow.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
namespace
{
const std::string NoFile = "<no file selected>";

bool isNum(const std::string& s) {
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

bool validFile(const std::string& f) {
    if (f.empty() || f == NoFile) { return false; }
    if (!bl::util::FileUtil::exists(bl::util::FileUtil::joinPath(core::Properties::NpcPath(), f))) {
        if (!bl::util::FileUtil::exists(
                bl::util::FileUtil::joinPath(core::Properties::TrainerPath(), f))) {
            return false;
        }
    }
    return true;
}

} // namespace

using namespace bl::gui;

CharacterSpawnWindow::CharacterSpawnWindow(const OnEdit& cb)
: onEdit(cb)
, npcEditor(std::bind(&CharacterSpawnWindow::onFilechoose, this, std::placeholders::_1),
            [this]() { window->setForceFocus(true); })
, trainerEditor(std::bind(&CharacterSpawnWindow::onFilechoose, this, std::placeholders::_1),
                [this]() { window->setForceFocus(true); }) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "Character Spawn");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) { closeAll(); });

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    fileLabel    = Label::create("file here");
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(fileLabel, true, true);
    Button::Ptr npcBut = Button::create("NPC");
    npcBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        std::string f = fileLabel->getText();
        if (bl::util::FileUtil::getExtension(f) != core::Properties::NpcFileExtension() ||
            !validFile(f)) {
            f.clear();
        }
        window->setForceFocus(false);
        npcEditor.show(parent, f);
    });
    row->pack(npcBut, false, true);
    Button::Ptr tnrBut = Button::create("Trainer");
    tnrBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        std::string f = fileLabel->getText();
        if (bl::util::FileUtil::getExtension(f) != core::Properties::TrainerFileExtension() ||
            !validFile(f)) {
            f.clear();
        }
        window->setForceFocus(false);
        trainerEditor.show(parent, f);
    });
    row->pack(tnrBut, false, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Level:"), false, true);
    levelInput = TextEntry::create();
    levelInput->setMode(TextEntry::Mode::Integer);
    row->pack(levelInput, true, false);
    window->pack(row, true, false);

    row            = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Box::Ptr input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("X:"), false, true);
    xInput = TextEntry::create();
    xInput->setMode(TextEntry::Mode::Integer);
    input->pack(xInput, true, true);
    row->pack(input, true, false);
    input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("Y:"), false, true);
    yInput = TextEntry::create();
    yInput->setMode(TextEntry::Mode::Integer);
    input->pack(yInput, true, true);
    row->pack(input, true, false);
    window->pack(row, true, false);

    dirEntry = ComboBox::create();
    dirEntry->addOption("Up");
    dirEntry->addOption("Right");
    dirEntry->addOption("Down");
    dirEntry->addOption("Left");
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Dir:"), false, true);
    row->pack(dirEntry, false, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr editBut = Button::create("Confirm");
    editBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (!validFile(fileLabel->getText())) {
            bl::dialog::tinyfd_messageBox(
                "Selelct Character", "Please select an NPC or trainer to spawn", "ok", "error", 1);
            return;
        }
        if (levelInput->getInput().empty() || !isNum(levelInput->getInput())) {
            bl::dialog::tinyfd_messageBox(
                "Bad Level", "Please enter a valid level", "ok", "error", 1);
            return;
        }
        if (xInput->getInput().empty() || !isNum(xInput->getInput())) {
            bl::dialog::tinyfd_messageBox(
                "Bad Position", "Please enter a valid x tile", "ok", "error", 1);
            return;
        }
        if (yInput->getInput().empty() || !isNum(yInput->getInput())) {
            bl::dialog::tinyfd_messageBox(
                "Bad Position", "Please enter a valid y tile", "ok", "error", 1);
            return;
        }
        closeAll();
        onEdit(orig,
               core::map::CharacterSpawn(
                   core::component::Position(
                       std::atoi(levelInput->getInput().c_str()),
                       sf::Vector2i(std::atoi(xInput->getInput().c_str()),
                                    std::atoi(yInput->getInput().c_str())),
                       static_cast<core::component::Direction>(dirEntry->getSelectedOption())),
                   fileLabel->getText()));
    });
    row->pack(editBut, false, true);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        closeAll();
    });
    row->pack(cancelBut, false, true);
    window->pack(row, true, false);
}

void CharacterSpawnWindow::open(bl::gui::GUI* p, unsigned int level,
                                const sf::Vector2i& pos, const core::map::CharacterSpawn* orig) {
    parent     = p;
    this->orig = orig;
    parent->pack(window);
    window->setForceFocus(true);

    if (orig) {
        fileLabel->setText(orig->file);
        xInput->setInput(std::to_string(orig->position.positionTiles().x));
        yInput->setInput(std::to_string(orig->position.positionTiles().y));
        levelInput->setInput(std::to_string(orig->position.level));
        dirEntry->setSelectedOption(static_cast<int>(orig->position.direction));
    }
    else {
        fileLabel->setText("<no file selected>");
        xInput->setInput(std::to_string(pos.x));
        yInput->setInput(std::to_string(pos.y));
        levelInput->setInput(std::to_string(level));
        dirEntry->setSelectedOption(0);
    }
}

void CharacterSpawnWindow::onFilechoose(const std::string& file) { fileLabel->setText(file); }

void CharacterSpawnWindow::closeAll() {
    window->remove();
    window->setForceFocus(false);
    npcEditor.hide();
}

} // namespace component
} // namespace editor
