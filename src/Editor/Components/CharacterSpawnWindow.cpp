#include <Editor/Components/CharacterSpawnWindow.hpp>

namespace editor
{
namespace component
{
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
    dirEntry->addOption("Left");
    dirEntry->addOption("Down");
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Dir:"), false, true);
    row->pack(dirEntry, false, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr editBut = Button::create("Confirm");
    editBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO - validate input and call cb
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
    parent = p;
    parent->pack(window);

    if (orig) {
        // TODO - populate
    }
    else {
        // TODO - defaults
    }
}

} // namespace component
} // namespace editor
