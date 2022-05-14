#ifndef EDITOR_COMPONENTS_LIGHTPREVIEW_HPP
#define EDITOR_COMPONENTS_LIGHTPREVIEW_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Helper component that displays an image with a light level overlaid on it
 *
 * @ingroup Components
 *
 */
class LightPreview : public bl::gui::Image {
public:
    using Ptr = std::shared_ptr<LightPreview>;

    /**
     * @brief Create a new LightPreview component
     *
     * @param size The preview size
     * @return Ptr The new component
     */
    static Ptr create(const sf::Vector2f& size);

    /**
     * @brief Sets the light level to display
     *
     */
    void setLightLevel(std::uint8_t level);

private:
    sf::RectangleShape cover;

    LightPreview(const sf::Vector2f& size);
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const bl::gui::Renderer& renderer) const override;
};

} // namespace component
} // namespace editor

#endif
