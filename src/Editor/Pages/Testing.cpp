#include <Editor/Pages/Testing.hpp>

namespace editor
{
namespace page
{
Testing::Testing(core::system::Systems& s)
: Page(s) {
    content =
        bl::gui::Box::create(bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical), "pages");
    content->pack(bl::gui::Label::create("Testing tool will go here"));
}

void Testing::update(float) {
    // TODO
}

} // namespace page
} // namespace editor
