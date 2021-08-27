#include <Editor/Components/NPCEditorWindow.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;
NpcEditorWindow::NpcEditorWindow(const SelectCb& cb)
: selectCb(cb)
, clean(true) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 8), "NPC Editor");
    window->getSignal(Action::Closed).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });

    Box::Ptr row       = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr newBut = Button::create("New");
    newBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    Button::Ptr openBut = Button::create("Open");
    openBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    saveBut = Button::create("Save");
    saveBut->setColor(sf::Color::Yellow, sf::Color::Blue);
    saveBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    fileLabel = Label::create("filename.npc");
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(newBut, false, true);
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
    row->pack(animBut, false, true);
    row->pack(animLabel, true, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr convBut = Button::create("Select Conversation");
    convBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO
    });
    convLabel = Label::create("conversation.conv");
    row->pack(convBut, false, true);
    row->pack(convLabel, true, true);
    window->pack(row, true, false);

    // TODO - behavior editor

    row                   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr selectBut = Button::create("Use NPC");
    selectBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        // TODO - validate and check dirty
        selectCb(fileLabel->getText());
    });
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });
    row->pack(selectBut, false, true);
    row->pack(cancelBut, false, true);
    window->pack(row, true, false);
}

void NpcEditorWindow::show(GUI::Ptr p, const std::string& file) {
    parent = p;
    if (!file.empty()) {
        // TODO - load and populate
    }
    parent->pack(window);
}

} // namespace component
} // namespace editor
