#include <Editor/Pages/Items.hpp>

namespace editor
{
namespace page
{
Items::Items(core::system::Systems& s)
: Page(s) {
    content =
        bl::gui::Box::create(bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical));
    content->pack(bl::gui::Label::create("Item DB editor will go here"));
}

void Items::update(float) {
    // TODO
}

} // namespace page
} // namespace editor
