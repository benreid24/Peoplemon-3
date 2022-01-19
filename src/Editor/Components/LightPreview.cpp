#include <Editor/Components/LightPreview.hpp>

#include <BLIB/Engine/Resources.hpp>

namespace editor
{
namespace component
{
namespace
{
bl::resource::Resource<sf::Texture>::Ref txtr =
    bl::engine::Resources::textures().load("EditorResources/lightpreview.png").data;
}

LightPreview::Ptr LightPreview::create(const sf::Vector2f& size) {
    return Ptr(new LightPreview(size));
}

LightPreview::LightPreview(const sf::Vector2f& size)
: Image(txtr)
, cover(size) {
    scaleToSize(size);
    cover.setFillColor(sf::Color::Transparent);
}

void LightPreview::setLightLevel(std::uint8_t a) {
    cover.setFillColor(sf::Color(0, 0, 0, 255 - a));
}

void LightPreview::doRender(sf::RenderTarget& target, sf::RenderStates states,
                            const bl::gui::Renderer& renderer) const {
    Image::doRender(target, states, renderer);
    states.transform.translate(getPosition());
    target.draw(cover, states);
}

} // namespace component
} // namespace editor
