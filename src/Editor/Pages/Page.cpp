#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
Page::Page(core::system::Systems& s)
: systems(s) {}

bl::gui::Box::Ptr Page::getContent() { return content; }

} // namespace page
} // namespace editor
