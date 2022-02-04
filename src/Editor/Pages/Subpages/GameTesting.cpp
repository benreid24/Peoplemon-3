#include <Editor/Pages/Subpages/GameTesting.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

GameTesting::GameTesting() {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    content->pack(Label::create("TODO"), true, true);
}

const Box::Ptr& GameTesting::getContent() const { return content; }

} // namespace page
} // namespace editor
