#include <Editor/Pages/Todo.hpp>

namespace editor
{
namespace page
{
Todo::Todo(core::system::Systems& s)
: Page(s) {
    content = bl::gui::Box::create(bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical));
    content->pack(bl::gui::Label::create("Work items marked for later will show up here"));
}

void Todo::update(float) {
    // TODO
}

} // namespace page
} // namespace editor
