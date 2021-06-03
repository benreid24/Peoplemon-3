#include <Editor/Pages/Moves.hpp>

namespace editor
{
namespace page
{
Moves::Moves(core::system::Systems& s)
: Page(s) {
    content =
        bl::gui::Box::create(bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical), "pages");
    content->pack(bl::gui::Label::create("Move DB editor will go here"));
}

void Moves::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
