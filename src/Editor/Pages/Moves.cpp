#include <Editor/Pages/Moves.hpp>

namespace editor
{
namespace page
{
Moves::Moves(core::system::Systems& s)
: Page(s) {
    content->pack(bl::gui::Label::create("Move DB editor will go here"));
}

void Moves::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
