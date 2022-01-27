#include <Editor/Pages/VariousEditors.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

namespace
{
void ignore() {}
} // namespace

VariousEditors::VariousEditors(core::system::Systems& s)
: Page(s)
, conversationEditor(std::bind(&ignore), std::bind(&ignore))
, npcEditor(std::bind(&ignore), std::bind(&ignore))
, trainerEditor(std::bind(&ignore), std::bind(&ignore))
, playlistEditor(std::bind(&ignore), std::bind(&ignore)) {
    content          = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    Button::Ptr open = Button::create("Conversation Editor");
    open->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&VariousEditors::openConvEditor, this));
    content->pack(open);

    open = Button::create("NPC Editor");
    open->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&VariousEditors::openNpcEditor, this));
    content->pack(open);

    open = Button::create("Trainer Editor");
    open->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&VariousEditors::openTrainerEditor, this));
    content->pack(open);

    open = Button::create("Playlist Editor");
    open->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&VariousEditors::openPlaylistEditor, this));
    content->pack(open);
}

void VariousEditors::update(float) {}

void VariousEditors::openConvEditor() { conversationEditor.open(parent, ""); }

void VariousEditors::openNpcEditor() { npcEditor.show(parent, ""); }

void VariousEditors::openTrainerEditor() { trainerEditor.show(parent, ""); }

void VariousEditors::openPlaylistEditor() { playlistEditor.open(parent, ""); }

} // namespace page
} // namespace editor
