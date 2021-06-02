#include <Editor/Pages/Credits.hpp>

namespace editor
{
namespace page
{
Credits::Credits(core::system::Systems& s)
: Page(s) {
    content->pack(bl::gui::Label::create("Credits editor will go here"));
}

void Credits::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
