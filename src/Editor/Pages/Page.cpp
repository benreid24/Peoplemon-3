#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
Page::Page(core::system::Systems& s)
: systems(s)
, content(
      bl::gui::Box::create(bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical), "pages")) {
    content->setExpandsWidth(true);
    content->setExpandsHeight(true);
}

bl::gui::Box::Ptr Page::getContent() { return content; }

} // namespace page
} // namespace editor
