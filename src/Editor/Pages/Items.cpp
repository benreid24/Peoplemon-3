#include <Editor/Pages/Items.hpp>

namespace editor
{
namespace page
{
Items::Items(core::system::Systems& s)
: Page(s) {
    content->pack(bl::gui::Label::create("Item DB editor will go here"));
}

void Items::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
