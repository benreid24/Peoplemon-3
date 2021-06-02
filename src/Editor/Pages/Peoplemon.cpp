#include <Editor/Pages/Peoplemon.hpp>

namespace editor
{
namespace page
{
Peoplemon::Peoplemon(core::system::Systems& s)
: Page(s) {
    content->pack(bl::gui::Label::create("Peoplemon DB editor will go here"));
}

void Peoplemon::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
