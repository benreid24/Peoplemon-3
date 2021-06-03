#include <Editor/Pages/Peoplemon.hpp>

namespace editor
{
namespace page
{
Peoplemon::Peoplemon(core::system::Systems& s)
: Page(s) {
    content =
        bl::gui::Box::create(bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical), "pages");
    content->pack(bl::gui::Label::create("Peoplemon DB editor will go here"));
}

void Peoplemon::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
