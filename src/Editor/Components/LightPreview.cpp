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
: Image(ImageManager::load("EditorResources/lightpreview.png")) {
    scaleToSize(size);
}

void LightPreview::setLightLevel(std::uint8_t a) {
    setColor(sf::Color(a, a, a, 255), sf::Color::Transparent);
}

} // namespace component
} // namespace editor
