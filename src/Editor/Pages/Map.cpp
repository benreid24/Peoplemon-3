#include <Editor/Pages/Map.hpp>

namespace editor
{
namespace page
{
Map::Map(core::system::Systems& s)
: Page(s) {
    content->pack(bl::gui::Label::create("Map editor will go here"));
}

void Map::update(float dt) {
    // TODO
}

} // namespace page
} // namespace editor
