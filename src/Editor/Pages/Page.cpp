#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
Page::Page(core::system::Systems& s)
: systems(s)
, parent(nullptr) {}

bl::gui::Box::Ptr Page::getContent() { return content; }

void Page::registerGui(bl::gui::GUI* p) { parent = p; }

} // namespace page
} // namespace editor
