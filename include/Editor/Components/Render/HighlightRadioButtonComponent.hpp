#ifndef EDITOR_COMPONENTS_RENDER_HIGHLIGHTRADIOBUTTONCOMPONENT_HPP
#define EDITOR_COMPONENTS_RENDER_HIGHLIGHTRADIOBUTTONCOMPONENT_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Basic/RadioButtonComponent.hpp>

namespace editor
{
namespace component
{
/// Contains renderer components for custom GUI elements
namespace rdr
{
/**
 * @brief Renderer component for HighlightRadioButton
 *
 * @ingroup UIComponents
 */
class HighlightRadioButtonComponent : public bl::gui::rdr::Component {
public:
    /**
     * @brief Creates the component
     */
    HighlightRadioButtonComponent();

    /**
     * @brief Destroys the component
     */
    virtual ~HighlightRadioButtonComponent() = default;

private:
    bl::gui::defcoms::RadioButtonComponent base;
    bl::gfx::Rectangle highlight;

    virtual void setVisible(bool visible) override;
    virtual void onElementUpdated() override;
    virtual void onRenderSettingChange() override;
    virtual bl::ecs::Entity getEntity() const override;
    virtual void doCreate(bl::engine::Engine& engine, bl::gui::rdr::Renderer& renderer) override;
    virtual void doSceneAdd(bl::rc::Overlay* overlay) override;
    virtual void doSceneRemove() override;
    virtual void handleAcquisition() override;
    virtual void handleMove() override;
    virtual sf::Vector2f getRequisition() const override;
};

} // namespace rdr
} // namespace component
} // namespace editor

#endif
