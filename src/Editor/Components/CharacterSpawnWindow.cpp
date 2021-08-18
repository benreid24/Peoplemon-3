#include <Editor/Components/CharacterSpawnWindow.hpp>

namespace editor
{
namespace component
{
namespace
{
bool isNum(const std::string& s) {
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}
} // namespace

using namespace bl::gui;

CharacterSpawnWindow::CharacterSpawnWindow(const OnEdit& cb)
: onEdit(cb) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "Character Spawn");
    window->getSignal(Action::Closed).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    fileLabel    = Label::create("file here");
    row->pack(fileLabel, true, true);
    Button::Ptr npcBut = Button::create("NPC");
    npcBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    row->pack(npcBut, false, true);
    Button::Ptr tnrBut = Button::create("Trainer");
    tnrBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    row->pack(tnrBut, false, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Level:"), false, true);
    levelInput = TextEntry::create();
    row->pack(levelInput, true, false);
    window->pack(row, true, false);

    row            = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Box::Ptr input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("X:"), false, true);
    xInput = TextEntry::create();
    input->pack(xInput, true, true);
    row->pack(input, true, false);
    input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("Y:"), false, true);
    yInput = TextEntry::create();
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
    editBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (fileLabel->getText().empty()) {
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
        window->remove();
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
    cancelBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });
    row->pack(cancelBut, false, true);
    window->pack(row, true, false);
}

void CharacterSpawnWindow::open(const bl::gui::GUI::Ptr& p, unsigned int level,
                                const sf::Vector2i& pos, const core::map::CharacterSpawn* orig) {
    parent     = p;
    this->orig = orig;
    parent->pack(window);

    if (orig) {
        fileLabel->setText(orig->file);
        xInput->setInput(std::to_string(orig->position.getValue().positionTiles().x));
        yInput->setInput(std::to_string(orig->position.getValue().positionTiles().y));
        levelInput->setInput(std::to_string(orig->position.getValue().level));
        dirEntry->setSelectedOption(static_cast<int>(orig->position.getValue().direction));
    }
    else {
        fileLabel->setText("mom1.npc");
        xInput->setInput(std::to_string(pos.x));
        yInput->setInput(std::to_string(pos.y));
        levelInput->setInput(std::to_string(level));
        dirEntry->setSelectedOption(0);
    }
}

} // namespace component
} // namespace editor
