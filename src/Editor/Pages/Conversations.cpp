#include <Editor/Pages/Conversations.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

namespace
{
void ignore() {}
} // namespace

Conversations::Conversations(core::system::Systems& s)
: Page(s)
, editor(std::bind(&ignore), std::bind(&ignore)) {
    content          = Box::create(LinePacker::create(LinePacker::Vertical));
    Button::Ptr open = Button::create("Open Editor");
    open->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Conversations::openEditor, this));
    content->pack(open);
}

void Conversations::update(float) {}

void Conversations::openEditor() { editor.open(parent, ""); }

} // namespace page
} // namespace editor
