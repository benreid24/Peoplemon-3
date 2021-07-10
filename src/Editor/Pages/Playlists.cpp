#include <Editor/Pages/Playlists.hpp>

namespace editor
{
namespace page
{
Playlists::Playlists(core::system::Systems& s)
: Page(s) {
    content =
        bl::gui::Box::create(bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical), "pages");
    content->pack(bl::gui::Label::create("Playlists tool will go here"));
}

void Playlists::update(float) {
    // TODO
}

} // namespace page
} // namespace editor
