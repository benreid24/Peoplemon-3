#include <Editor/Components/LightPreview.hpp>

#include <Core/Resources.hpp>

namespace editor
{
namespace component
{
LightPreview::Ptr LightPreview::create(const sf::Vector2f& size) {
    return Ptr(new LightPreview(size));
}

LightPreview::LightPreview(const sf::Vector2f& size)
: Image(ImageManager::load("EditorResources/lightpreview.png"))
, cover(size) {
    scaleToSize(size);
    cover.setFillColor(sf::Color::Transparent);
}

bl::gui::rdr::Component* LightPreview::doPrepareRender(bl::gui::rdr::Renderer& renderer) {
    return nullptr;
}

void LightPreview::setLightLevel(std::uint8_t a) {
    cover.setFillColor(sf::Color(0, 0, 0, 255 - a));
}

// void LightPreview::doRender(sf::RenderTarget& target, sf::RenderStates states,
//                             const bl::gui::Renderer& renderer) const {
//     Image::doRender(target, states, renderer);
//     states.transform.translate(getPosition());
//     target.draw(cover, states);
// }

} // namespace component
} // namespace editor
