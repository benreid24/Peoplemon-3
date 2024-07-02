#ifndef EDITOR_COMPONENTS_RENDER_CONVERSATIONTREECOMPONENT_HPP
#define EDITOR_COMPONENTS_RENDER_CONVERSATIONTREECOMPONENT_HPP

#include <BLIB/Graphics/BatchCircle.hpp>
#include <BLIB/Graphics/BatchIcon.hpp>
#include <BLIB/Graphics/BatchedShapes2D.hpp>
#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Graphics/Text.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <vector>

namespace editor
{
namespace component
{
namespace rdr
{
/**
 * @brief GUI renderer component for the conversation tree
 *
 * @ingroup UIComponents
 */
class ConversationTreeComponent : public bl::gui::rdr::Component {
public:
    /**
     * @brief Creates the component
     */
    ConversationTreeComponent();

    /**
     * @brief Destroys the component
     */
    virtual ~ConversationTreeComponent() = default;

private:
    bl::gfx::Rectangle background;
    bl::gfx::BatchedShapes2D shapeBatch;
    std::vector<bl::gfx::BatchCircle> nodeCircles;
    std::vector<bl::gfx::BatchIcon> nodeArrows;
    std::vector<bl::gfx::Text> nodeLabels;

    bl::engine::Engine* enginePtr;
    bl::rc::Overlay* currentOverlay;

    virtual void setVisible(bool visible) override;
    virtual void onElementUpdated() override;
    virtual void onRenderSettingChange() override;
    virtual bl::ecs::Entity getEntity() const override;
    virtual void doCreate(bl::engine::Engine& engine, bl::gui::rdr::Renderer& renderer) override;
    virtual void doSceneAdd(bl::rc::Overlay* overlay) override;
    virtual void doSceneRemove() override;
    virtual void handleAcquisition() override;
    virtual void handleMove() override;
};
} // namespace rdr
} // namespace component
} // namespace editor

#endif
