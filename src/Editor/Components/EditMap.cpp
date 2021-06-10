#include <Editor/Components/EditMap.hpp>

namespace editor
{
namespace component
{
EditMap::Ptr EditMap::create(const ClickCb& clickCb) { return Ptr(new EditMap(clickCb)); }

EditMap::EditMap(const ClickCb& clickCb)
: bl::gui::Element("maps", "editmap")
, clickCb(clickCb) {
    // TODO - init stuff
}

sf::Vector2i EditMap::minimumRequisition() const { return {100, 100}; }

void EditMap::doRender(sf::RenderTarget& target, sf::RenderStates states,
                       const bl::gui::Renderer& renderer) const {
    // TODO
}

bool EditMap::handleScroll(const bl::gui::Action& scroll) {
    // TODO
    return true;
}

} // namespace component
} // namespace editor
