#include <Editor/Pages/Testing.hpp>

namespace editor
{
namespace page
{
Testing::Testing(core::system::Systems& s)
: Page(s) {
    content->pack(bl::gui::Label::create("Testing tool will go here"));
}

void Testing::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
